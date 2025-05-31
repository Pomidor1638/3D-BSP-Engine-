#include "cl_user.h"
#include "world.h"

void SolvePlanes(std::vector<plane_s>& planes, vec3_t vel, vec3_t result) {

}

// Обновлённая функция движения с поддержкой многоплоскостных отражений
void Movement(entity* p, vec_t dt, bspmap_t* tree) {
    const float speed = 100.0f;
    const int max_bumps = 4;
    int i, j;

    vec3_t angle = p->GetAngle();
    vec3_t start = p->GetPos();
    vec3_t vel = p->GetVel();
    vec3_t end;
    vec3_t wishdir = { 0, 0, 0 };

    vec_t cos_z = cos(angle[2]), sin_z = sin(angle[2]);

    if (GetKeyState('W') < 0) {
        wishdir[0] -= sin_z;
        wishdir[1] += cos_z;
    }
    if (GetKeyState('D') < 0) {
        wishdir[0] += cos_z;
        wishdir[1] += sin_z;
    }
    if (GetKeyState('S') < 0) {
        wishdir[0] += sin_z;
        wishdir[1] -= cos_z;
    }
    if (GetKeyState('A') < 0) {
        wishdir[0] -= cos_z;
        wishdir[1] -= sin_z;
    }
    wishdir = normalize(wishdir);

    if (GetKeyState('X') < 0) {
        p->SetVel({ 0, 0, 0 });
        p->SetPos(start + 2 * speed * wishdir * dt);
        return;
    }

    if (GetKeyState(VK_SPACE) < 0)
        vel[2] += 2 * speed * dt;
    else
        vel[2] -= speed * dt;

    if (GetKeyState(VK_LSHIFT) < 0)
        vel[2] -= 2 * speed * dt;

    SV_Accelerate(speed, wishdir, vel, dt);

    std::vector<vec3_t> clip_planes;

    vec3_t original_velocity = vel;
    vec3_t primal_velocity = vel;

    vec3_t new_vel;
    float time_left = dt;

    for (int bump = 0; bump < max_bumps; ++bump) {
        if (vel == vec3_t{ 0, 0, 0 }) {
            break;
        }

        end = start + vel * time_left;

        trace_s trace = SV_Move(tree, start, end);

        if (trace.allsolid) {
            vel = vec3_t{ 0, 0, 0 };
            p->SetVel(vel);
            return;
        }

        if (trace.fraction > 0) {
            start = trace.endpos;
            original_velocity = vel;
            clip_planes = {};
        }

        if (trace.fraction == 1)
            break;

        if (trace.plane.normal[2] > 0.7) {
            p->floor = true;
        }
        
        time_left -= time_left * trace.fraction;

        if (clip_planes.size() >= 5) {	
            vel = { 0, 0, 0 };
            p->SetVel(vel);
            return;
        }

        clip_planes.push_back(normalize(trace.plane.normal));

        for (i = 0; i < clip_planes.size(); i++) {
            ClipVelocity(original_velocity, clip_planes[i], new_vel, 1.01f);
            for (j = 0; j < clip_planes.size(); j++) {
                if (i != j) {
                    if (new_vel * clip_planes[j] < 0)
                        break;
                }
            }
            if (j == clip_planes.size())
                break;
        }

        if (i != clip_planes.size()) {
            vel = new_vel;
        }
        else {
            if (clip_planes.size() != 2) {
                vel = { 0, 0, 0 };
                p->SetVel(vel);
                return;
            }
            vec3_t dir = Cross(clip_planes[0], clip_planes[1]);
            float len = dir.length();
            if (len < 1e-5f) {
                vel = { 0, 0, 0 };
                p->SetVel(vel);
                return;
            }
            dir *= 1.0 / len;
            vec_t d = DotProduct(vel, dir);
            vel = d * dir;

        }

        if (vel * primal_velocity <= 0) {
            vel = { 0, 0, 0 };
            p->SetVel(vel);
            return;
        }
    }

    p->SetVel(vel);
    p->SetPos(end);
}

void MoveCamera(vec3_t angle, vec3_t pos) {
	glRotatef(degrees(-angle[0]), 1, 0, 0);
	glRotatef(degrees(-angle[2]), 0, 0, 1);
	glRotatef(degrees(-angle[1]), 0, 1, 0);
	glTranslatef(-pos[0], -pos[1], -pos[2]);
};

void MovePlayerLook(entity* p, const HWND& hwnd, unsigned int width, unsigned int height) {
	if (GetForegroundWindow() != hwnd) return;
	POINT cur;
	static const POINT base = { width >> 1, height >> 1 };
	GetCursorPos(&cur);
	vec_t dx = (base.x - cur.x) * 0.001;
	vec_t dy = (base.y - cur.y) * 0.001;
	p->Rotate({ dy, 0 , dx });
	SetCursorPos(base.x, base.y);
}
