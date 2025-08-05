// pong_rt3d.c
//
// Pong clone with full 3D ray‐traced perspective, rotating camera,
// light blue sky, light orange floor, slower orbit, and paddle‐limit stripes.

#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/event.h>
#include <banjo/linmath.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/shader.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>
#include <banjo/math.h>    // bj_clamp
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Compile‐time constants
#define SCREEN_W          800
#define SCREEN_H          600
#define BALL_SIZE         16.0f
#define PADDLE_MARGIN     50.0f
#define PADDLE_LENGTH     120.0f
#define PADDLE_WIDTH      24.0f
#define PADDLE_HEIGHT     80.0f
#define PADDLE_VELOCITY   (250.0f)

// Precomputed positions
#define LEFT_PADDLE_X     (PADDLE_MARGIN)
#define RIGHT_PADDLE_X    (SCREEN_W - PADDLE_MARGIN - PADDLE_WIDTH)

////////////////////////////////////////////////////////////////////////////////
// Game state
static struct {
    bj_vec2   ball_pos;
    bj_vec2   ball_vel;
    float     paddle_y[2];
    bj_bool   paddle_up[2];
    bj_bool   paddle_down[2];
    bj_bool   running;
} game;

static bj_window*   window      = NULL;
static bj_bitmap*   framebuffer = NULL;
static bj_stopwatch stopwatch    = {0};
static float        time_secs    = 0.0f;

////////////////////////////////////////////////////////////////////////////////
// Sphere intersection
static int intersect_sphere(const bj_vec3 ro,const bj_vec3 rd,
                            const bj_vec3 center,float radius,
                            float *t_out,bj_vec3 n_out)
{
    bj_vec3 oc; bj_vec3_sub(oc, ro, center);
    float b = bj_vec3_dot(oc, rd);
    float c = bj_vec3_dot(oc, oc) - radius*radius;
    float disc = b*b - c;
    if (disc < 0) return 0;
    float sq = sqrtf(disc);
    float t = -b - sq;
    if (t <= 0) t = -b + sq;
    if (t <= 0) return 0;
    *t_out = t;
    // normal
    bj_vec3 P; bj_vec3_scale(P, rd, t); bj_vec3_add(P, ro, P);
    bj_vec3_sub(n_out, P, center);
    bj_vec3_scale(n_out, n_out, 1.0f/radius);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Box (AABB) intersection
static int intersect_box(const bj_vec3 ro,const bj_vec3 rd,
                         const bj_vec3 bmin,const bj_vec3 bmax,
                         float *t_out,bj_vec3 n_out)
{
    float tmin = -INFINITY, tmax = INFINITY;
    bj_vec3 best_n = {0,0,0};
    for (int i=0;i<3;++i) {
        if (fabsf(rd[i]) < 1e-6f) {
            if (ro[i] < bmin[i]||ro[i]>bmax[i]) return 0;
        } else {
            float ood=1.0f/rd[i];
            float t1=(bmin[i]-ro[i])*ood;
            float t2=(bmax[i]-ro[i])*ood;
            if (t1>t2) { float tmp=t1; t1=t2; t2=tmp; }
            if (t1>tmin) {
                tmin=t1;
                best_n[0]=best_n[1]=best_n[2]=0.0f;
                best_n[i]=(rd[i]>0? -1.0f:+1.0f);
            }
            tmax=fminf(tmax,t2);
            if (tmin>tmax) return 0;
        }
    }
    if (tmin<0) return 0;
    *t_out=tmin;
    bj_vec3_copy(n_out,best_n);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Ray‐tracing shader, 3D scene
int pong_rt3d_shader(bj_vec3 out_color, const bj_vec2 pix, void* ud) {
    float t = *(float*)ud;

    // 1) Camera ray in NDC
    float ndc_x = (pix[0]/(SCREEN_W-1))*2.0f - 1.0f;
    float ndc_y = 1.0f - (pix[1]/(SCREEN_H-1))*2.0f;
    float aspect = (float)SCREEN_W/(float)SCREEN_H;
    bj_vec3 rd_cam = { ndc_x*aspect, ndc_y, 1.0f };
    bj_vec3_normalize(rd_cam, rd_cam);

    // 2) Orbiting camera
    float ang = t * 0.05f;            // << slowed to 0.05 rad/s
    bj_vec3 cam_pos, target={SCREEN_W/2,SCREEN_H/2,0}, up={0,0,1};
    cam_pos[0]=SCREEN_W/2 + cosf(ang)*600.0f;
    cam_pos[1]=SCREEN_H/2 + sinf(ang)*600.0f;
    cam_pos[2]=400.0f;

    // 3) Build coordinate frame
    bj_vec3 forward,tmp,right,cam_up,rd,ro;
    bj_vec3_sub(tmp,target,cam_pos);
    bj_vec3_normalize(forward,tmp);
    bj_vec3_cross(right,forward,up);
    bj_vec3_normalize(right,right);
    bj_vec3_cross(cam_up,right,forward);

    // 4) Ray in world
    bj_vec3_scale(rd,right,   rd_cam[0]);
    bj_vec3_scale(tmp, cam_up,rd_cam[1]); bj_vec3_add(rd,rd,tmp);
    bj_vec3_scale(tmp, forward,rd_cam[2]); bj_vec3_add(rd,rd,tmp);
    bj_vec3_normalize(rd,rd);
    bj_vec3_copy(ro,cam_pos);

    // 5) Trace
    float t_hit=1e30f; bj_vec3 norm={0,0,0}, col_sky={0.5f,0.7f,1.0f};

    // --- floor plane at z=0, light orange ---
    if (rd[2]<-1e-6f) {
        float tf = -ro[2]/rd[2];
        if (tf>0 && tf<t_hit) {
            t_hit = tf;
            bj_vec3 PF; bj_vec3_scale(PF, rd, tf); bj_vec3_add(PF, ro, PF);
            // base floor color
            col_sky[0]=0.8f; col_sky[1]=0.5f; col_sky[2]=0.2f;
            // paddle‐limit stripes
            float dxL = fabsf(PF[0] - (LEFT_PADDLE_X + PADDLE_WIDTH*0.5f));
            float dxR = fabsf(PF[0] - (RIGHT_PADDLE_X + PADDLE_WIDTH*0.5f));
            if (dxL<5.0f || dxR<5.0f) {
                col_sky[0]=1.0f; col_sky[1]=0.2f; col_sky[2]=0.2f;
            }
            bj_vec3_set(norm,0,0,1);
        }
    }

    // --- ball ---
    {
        bj_vec3 ball_c={game.ball_pos[0],game.ball_pos[1],BALL_SIZE*0.5f};
        float ts; bj_vec3 n2;
        if (intersect_sphere(ro,rd,ball_c,BALL_SIZE*0.5f,&ts,n2)&&ts<t_hit) {
            t_hit=ts;
            bj_vec3_set(col_sky,1,1,1);
            bj_vec3_copy(norm,n2);
        }
    }

    // --- paddles ---
    for (int i=0;i<2;++i) {
        bj_vec3 bmin={ i?RIGHT_PADDLE_X:LEFT_PADDLE_X,
                       game.paddle_y[i]-PADDLE_LENGTH*0.5f,
                       0 };
        bj_vec3 bmax={ bmin[0]+PADDLE_WIDTH,
                       bmin[1]+PADDLE_LENGTH*0.5f,
                       PADDLE_HEIGHT };
        float ts; bj_vec3 n2;
        if (intersect_box(ro,rd,bmin,bmax,&ts,n2)&&ts<t_hit) {
            t_hit=ts;
            if (!i) bj_vec3_set(col_sky,1,1,0.2f);
            else    bj_vec3_set(col_sky,0.2f,0.2f,1);
            bj_vec3_copy(norm,n2);
        }
    }

    // 6) Lighting & shadows
    if (t_hit<1e29f) {
        bj_vec3 P,L,shadow_ro,negL; bj_vec3_scale(P,rd,t_hit); bj_vec3_add(P,ro,P);
        bj_vec3_set(L,-0.5f,-0.5f,1.0f); bj_vec3_normalize(L,L);
        float diff=bj_clamp(bj_vec3_dot(norm,L),0.1f,1.0f);
        // shadow...
        bj_vec3_scale(shadow_ro,norm,1e-3f); bj_vec3_add(shadow_ro,P,shadow_ro);
        bj_vec3_scale(negL,L,-1.0f);
        int in_shadow=0; {
            float ts2; bj_vec3 dummy;
            bj_vec3 ball_c={game.ball_pos[0],game.ball_pos[1],BALL_SIZE*0.5f};
            in_shadow|=intersect_sphere(shadow_ro,negL,ball_c,BALL_SIZE*0.5f,&ts2,dummy);
            bj_vec3 bmin0={LEFT_PADDLE_X,game.paddle_y[0]-PADDLE_LENGTH*0.5f,0};
            bj_vec3 bmax0={LEFT_PADDLE_X+PADDLE_WIDTH,game.paddle_y[0]+PADDLE_LENGTH*0.5f,PADDLE_HEIGHT};
            in_shadow|=intersect_box(shadow_ro,negL,bmin0,bmax0,&ts2,dummy);
            bj_vec3 bmin1={RIGHT_PADDLE_X,game.paddle_y[1]-PADDLE_LENGTH*0.5f,0};
            bj_vec3 bmax1={RIGHT_PADDLE_X+PADDLE_WIDTH,game.paddle_y[1]+PADDLE_LENGTH*0.5f,PADDLE_HEIGHT};
            in_shadow|=intersect_box(shadow_ro,negL,bmin1,bmax1,&ts2,dummy);
        }
        float shade = diff * (in_shadow?0.3f:1.0f);
        out_color[0]=bj_clamp(col_sky[0]*shade,0,1);
        out_color[1]=bj_clamp(col_sky[1]*shade,0,1);
        out_color[2]=bj_clamp(col_sky[2]*shade,0,1);
    } else {
        // sky
        out_color[0]=0.5f;
        out_color[1]=0.7f;
        out_color[2]=1.0f;
    }

    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Input
static bj_bool use_shader = BJ_TRUE;
static void key_callback(bj_window* w, const bj_key_event* e) {
    (void)w;
    const struct { bj_key up,down; } map[2] = {
        { BJ_KEY_D, BJ_KEY_F }, { BJ_KEY_K, BJ_KEY_J }
    };
    for (int i=0;i<2;++i) {
        if (e->key==map[i].up)
            game.paddle_up[i]=(e->action==BJ_PRESS), game.running= BJ_TRUE;
        if (e->key==map[i].down)
            game.paddle_down[i]=(e->action==BJ_PRESS), game.running= BJ_TRUE;
    }
    switch(e->key) {
      case BJ_KEY_ESCAPE:
        if (e->action==BJ_RELEASE) bj_window_set_should_close(w);
        break;
      case BJ_KEY_SPACE:
        if (e->action==BJ_PRESS) use_shader = !use_shader;
        break;
      default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Reset & update
static void reset_game() {
    bj_vec2_set(game.ball_pos, SCREEN_W/2, SCREEN_H/2);
    bj_vec2_set(game.ball_vel, 200,200);
    for (int i=0;i<2;++i) {
        game.paddle_y[i]=SCREEN_H/2;
        game.paddle_up[i]=game.paddle_down[i]=BJ_FALSE;
    }
    game.running= BJ_FALSE;
}

static void update_game(double dt) {
    time_secs += (float)dt;
    float half=BALL_SIZE*0.5f;
    bj_vec2 np; bj_vec2_add_scaled(np, game.ball_pos, game.ball_vel, dt);

    if (np[1]<half||np[1]>SCREEN_H-half) {
        np[1]=bj_clamp(np[1],half,SCREEN_H-half);
        game.ball_vel[1] = -game.ball_vel[1];
    }
    float speed=bj_vec2_len(game.ball_vel);
    for (int i=0;i<2;++i) {
        float cx=(i==0?LEFT_PADDLE_X:RIGHT_PADDLE_X)+PADDLE_WIDTH*0.5f;
        if (fabsf(np[0]-cx)<=half+PADDLE_WIDTH*0.5f &&
            fabsf(np[1]-game.paddle_y[i])<=half+PADDLE_LENGTH*0.5f)
        {
            float base=(i==0?0.0f:BJ_PI),
                  rnd=(rand()/(float)RAND_MAX)*2.0f-1.0f,
                  ang=base+rnd*(BJ_PI/4);
            game.ball_vel[0]=cosf(ang)*speed;
            game.ball_vel[1]=sinf(ang)*speed;
        }
    }
    bj_vec2_copy(game.ball_pos,np);
    if (game.ball_pos[0]<-half||game.ball_pos[0]>SCREEN_W+half){
        bj_info("Point!"); reset_game();
    }
    for (int i=0;i<2;++i){
        float dir=(float)game.paddle_down[i]-(float)game.paddle_up[i];
        game.paddle_y[i]= bj_clamp(
            game.paddle_y[i]+dir*(float)dt*PADDLE_VELOCITY,
            PADDLE_LENGTH*0.5f,
            SCREEN_H-PADDLE_LENGTH*0.5f
        );
    }
}

////////////////////////////////////////////////////////////////////////////////
// App callbacks
int bj_app_begin(void** ud,int argc,char*argv[]){
    (void)ud;(void)argc;(void)argv;
    bj_error* err=NULL;
    if(!bj_begin(&err)){ bj_err("Err 0x%X:%s",err->code,err->message); return bj_callback_exit_error;}
    window      = bj_window_new("Pong 3D RT",0,0,SCREEN_W,SCREEN_H,0);
    framebuffer = bj_window_get_framebuffer(window,0);
    bj_set_key_callback(key_callback);
    reset_game();
    return bj_callback_continue;
}
int bj_app_iterate(void*ud){
    (void)ud; bj_dispatch_events();
    update_game(bj_stopwatch_step_delay(&stopwatch));
    if(use_shader){
        bj_bitmap_apply_shader(framebuffer,pong_rt3d_shader,&time_secs,0);
    } else {
        bj_bitmap_clear(framebuffer);
    }
    bj_window_update_framebuffer(window);
    bj_sleep(15);
    return bj_window_should_close(window)
        ? bj_callback_exit_success
        : bj_callback_continue;
}
int bj_app_end(void*ud,int s){ (void)ud; bj_window_del(window); bj_end(0); return s; }
