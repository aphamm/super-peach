#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

const int BLOCK_NONE = 0;
const int BLOCK_FLOWER = 1;
const int BLOCK_STAR = 2;
const int BLOCK_MUSHROOM = 3;

class StudentWorld;

////////////////////////////////////////////////////
// -------------------- ACTOR ----------------------
////////////////////////////////////////////////////

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, int x, int y,
          int dir, int depth, double size) :
        GraphObject(imageID, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, dir, depth, size),
        m_world(world), m_alive(true) {};
    
    virtual void doSomething() { return; };
    virtual bool blockActor() { return false; };
    virtual void bonk() { return; };
    virtual void getDamaged() { return; };
    virtual bool isLethal() { return false; };
    virtual bool isDamageable() { return false; };

    StudentWorld* getWorld() { return m_world; };
    bool isAlive() { return m_alive; };
    void setDead() { m_alive = false; };
    
private:
    StudentWorld* m_world;
    bool m_alive;
};

////////////////////////////////////////////////////
// -------------------- PEACH ----------------------
////////////////////////////////////////////////////

class Alive : public Actor
{
public:
    Alive(StudentWorld* world, int n, int x, int y,
        int dir, int depth, double size) :
        Actor(world, n, x, y, dir, depth, size) {};
    virtual bool isDamageable() { return true; };
};

class Peach : public Alive
{
public:
    Peach(StudentWorld* world, int x, int y) :
        Alive(world, IID_PEACH, x, y, 0, 0, 1.0),
        m_star(false), m_shoot(false), m_jump(false), m_invincible(0),
        m_health(1), m_recharge(0), m_remaining_jump_distance(0) {};
    virtual void doSomething();
    virtual void bonk();
    
    // mutators
    void setStar(bool state) { m_star = state; };
    void setShoot(bool state) { m_shoot = state; };
    void setJump(bool state) { m_jump = state; };
    void setInvincible(int ticks) { m_invincible = ticks; };
    void setHealth(int health) { m_health = health; };
    
    // accessors
    bool hasStar() { return m_star; };
    bool hasShoot() { return m_shoot; };
    bool hasJump() { return m_jump; };
    bool isInvincible() { return m_invincible; };
    
private:
    bool m_star;
    bool m_shoot;
    bool m_jump;
    int m_invincible;
    int m_health;
    int m_recharge;
    int m_remaining_jump_distance;
};

////////////////////////////////////////////////////
// -------------------- ENEMY ----------------------
////////////////////////////////////////////////////

class Enemy : public Alive
{
public:
    Enemy(StudentWorld* world, int n, int x, int y) :
        Alive(world, n, x, y, randInt(0, 1)*180, 1, 0) {} ;
    virtual bool isLethal() { return true; };
    virtual void doSomething();
    virtual void bonk();
    virtual void getDamaged();
private:
    virtual void deadKoop() { return; };
};

class Goom : public Enemy
{
public:
    Goom(StudentWorld* world, int x, int y) :
        Enemy(world, IID_GOOMBA, x, y) {};
};

class Koop : public Enemy
{
public:
    Koop(StudentWorld* world, int x, int y) :
        Enemy(world, IID_KOOPA, x, y) {};
private:
    virtual void deadKoop();
};

class Pira : public Enemy
{
public:
    Pira(StudentWorld* world, int x, int y) :
        Enemy(world, IID_PIRANHA, x, y),
        m_fire_delay(0) {};
    virtual void doSomething();
private:
    int m_fire_delay;
};

/////////////////////////////////////////////////////
// -------------------- PROJECTILE ------------------
/////////////////////////////////////////////////////

class Projectile : public Actor
{
public:
    Projectile(StudentWorld* world, int n, int x, int y, int dir) :
        Actor(world, n, x, y, dir, 1, 1) {} ;
    virtual void doSomething();
private:
    virtual bool shouldDamage(int x, int y);
    virtual void damageSomething(int x, int y);
};

class PiraBall : public Projectile
{
public:
    PiraBall(StudentWorld* world, int x, int y, int dir) :
        Projectile(world, IID_PIRANHA_FIRE, x, y, dir) {};
private:
    virtual bool shouldDamage(int x, int y);
    virtual void damageSomething(int x, int y);
};

class PeachBall : public Projectile
{
public:
    PeachBall(StudentWorld* world, int x, int y, int dir) :
        Projectile(world, IID_PEACH_FIRE, x, y, dir) {};
};

class Shell : public Projectile
{
public:
    Shell(StudentWorld* world, int x, int y, int dir) :
        Projectile(world, IID_SHELL, x, y, dir) {};
};

/////////////////////////////////////////////////////
// -------------------- OBJECT ----------------------
/////////////////////////////////////////////////////

class Solid : public Actor
{
public:
    Solid(StudentWorld* world, int n, int x, int y) :
        Actor(world, n, x, y, 0, 2, 1) {};
    virtual bool blockActor() { return true; };
};

class Block : public Solid
{
public:
    Block(StudentWorld* world, int x, int y, int type) :
        Solid(world, IID_BLOCK, x, y), m_goodie(type) {};
    virtual void bonk();
private:
    int m_goodie;
};

class Pipe : public Solid
{
public:
    Pipe(StudentWorld* world, int x, int y) :
        Solid(world, IID_PIPE, x, y) {};
};

class LevelChange : public Actor
{
public:
    LevelChange(StudentWorld* world, int n, int x, int y) :
        Actor(world, n, x, y, 0, 0, 1) {};
    virtual void doSomething();
private:
    virtual void levelCompleted() = 0;
};

class Flag : public LevelChange
{
public:
    Flag(StudentWorld* world, int x, int y) :
        LevelChange(world, IID_FLAG, x, y) {};
private:
    virtual void levelCompleted();
};

class Mario : public LevelChange
{
public:
    Mario(StudentWorld* world, int x, int y) :
        LevelChange(world, IID_MARIO, x, y) {};
private:
    virtual void levelCompleted();
};

/////////////////////////////////////////////////////
// -------------------- GOODIE ----------------------
/////////////////////////////////////////////////////

class Goodie : public Actor
{
public:
    Goodie(StudentWorld* world, int n, int x, int y) :
        Actor(world, n, x, y, 0, 1, 1) {} ;
    virtual void doSomething();
private:
    virtual void goodieConsumed() = 0;
};

class Flower : public Goodie
{
public:
    Flower(StudentWorld* world, int x, int y) :
        Goodie(world, IID_FLOWER, x, y) {} ;
private:
    virtual void goodieConsumed();
};

class Mushroom : public Goodie
{
public:
    Mushroom(StudentWorld* world, int x, int y) :
        Goodie(world, IID_MUSHROOM, x, y) {} ;
private:
    virtual void goodieConsumed();
};

class Star : public Goodie
{
public:
    Star(StudentWorld* world, int x, int y) :
        Goodie(world, IID_STAR, x, y) {} ;
private:
    virtual void goodieConsumed();
};

#endif // ACTOR_H_
