#include "Actor.h"
#include "StudentWorld.h"
#include "Level.h"

////////////////////////////////////////////////////
// -------------------- PEACH ----------------------
////////////////////////////////////////////////////

void Peach::doSomething()
{
    if (!isAlive()) return;
    if (m_invincible > 0) m_invincible--;
    if (m_invincible == 0) m_star = false;
    if (m_recharge > 0) m_recharge--;
    
    getWorld()->bonkActors(getX(), getY());
    
    if (m_remaining_jump_distance > 0)
    {
        if (getWorld()->attemptMove(getX(), getY()+4))
        {
            moveTo(getX(), getY()+4);
            m_remaining_jump_distance--;
        } else {
            getWorld()->bonkActors(getX(), getY()+4);
            m_remaining_jump_distance = 0;
        }
    } else if (!getWorld()->hasSolidBelow(getX(), getY(), 3))
        moveTo(getX(), getY()-4);
        
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                setDirection(180);
                if (getWorld()->attemptMove(getX()-4, getY()))
                    moveTo(getX()-4, getY());
                else
                    getWorld()->bonkActors(getX()-4, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(0);
                if (getWorld()->attemptMove(getX()+4, getY()))
                    moveTo(getX()+4, getY());
                else
                    getWorld()->bonkActors(getX()+4, getY());
                break;
            case KEY_PRESS_UP:
                if(getWorld()->hasSolidBelow(getX(), getY(), 1))
                {
                    if (m_jump)
                        m_remaining_jump_distance = 12;
                    else
                        m_remaining_jump_distance = 8;
                    getWorld()->playSound(SOUND_PLAYER_JUMP);
                }
                break;
            case KEY_PRESS_SPACE:
                if (m_shoot && m_recharge == 0)
                {
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    m_recharge = 8;
                    if (getDirection() == 0)
                        getWorld()->addPeachBall((getX()+4)/SPRITE_WIDTH, getY()/SPRITE_HEIGHT, 0);
                    else
                        getWorld()->addPeachBall((getX()-4)/SPRITE_WIDTH, getY()/SPRITE_HEIGHT, 180);
                }
                break;
        }
    }
}

void Peach::bonk() // treat this as DAMAGE
{
    if (m_invincible > 0)
        return;
    
    m_health--;
    m_invincible = 10;
    m_shoot = false;
    m_jump = false;
    
    if (m_health == 1)
        getWorld()->playSound(SOUND_PLAYER_HURT);
    else
        setDead();
}

////////////////////////////////////////////////////
// -------------------- ENEMY ----------------------
////////////////////////////////////////////////////

void Enemy::doSomething()
{
    if(!isAlive()) return;
    
    if (getWorld()->overlapsPeach(getX(), getY()))
    {
        getWorld()->bonkPeach(getX(), getY());
        return;
    }
    
    int move = 1;
    if (getDirection() == 180)
        move = -1;
    
    if (getWorld()->attemptMove(getX()+move, getY()) &&
        getWorld()->wontFall(getX()+move, getY(), getDirection()))
        moveTo(getX()+move, getY());
    else
        move == 1 ? setDirection(180) : setDirection(0);
}

void Enemy::bonk()
{
    if (getWorld()->overlapsPeach(getX(), getY()))
        if (getWorld()->hasStar())
        {
            getWorld()->playSound(SOUND_PLAYER_KICK);
            getWorld()->increaseScore(100);
            setDead();
            deadKoop();
        }
}

void Enemy::getDamaged()
{
    getWorld()->increaseScore(100);
    setDead();
    deadKoop();
}

void Pira::doSomething()
{
    if(!isAlive()) return;
    
    increaseAnimationNumber();
    
    if (getWorld()->overlapsPeach(getX(), getY()))
    {
        getWorld()->bonkPeach(getX(), getY());
        return;
    }
    
    if (!getWorld()->peachLevel(getY()))
        return;
    
    int dir = 0;
    if(getWorld()->peachDistance(getX()) < 0)
        dir = 180;
        
    setDirection(dir);
    
    if (m_fire_delay > 0)
    {
        m_fire_delay--;
        return;
    }
    
    if (getWorld()->peachDistance(getX()) < 8 * SPRITE_WIDTH &&
        getWorld()->peachDistance(getX()) > -8 * SPRITE_WIDTH)
    {
        getWorld()->addPiraBall(getX()/SPRITE_WIDTH, getY()/SPRITE_HEIGHT, getDirection());
        getWorld()->playSound(SOUND_PIRANHA_FIRE);
        m_fire_delay = 40;
    }
}

void Koop::deadKoop()
{
    getWorld()->addShell(getX()/SPRITE_WIDTH, getY()/SPRITE_HEIGHT, getDirection());
}

/////////////////////////////////////////////////////
// -------------------- PROJECTILE ------------------
/////////////////////////////////////////////////////

void Projectile::doSomething()
{
    if(shouldDamage(getX(), getY()))
    {
        damageSomething(getX(), getY());
        setDead();
        return;
    } else if (!getWorld()->hasSolidBelow(getX(), getY(), 2))
        moveTo(getX(), getY()-2);
    
    int move = 2;
    if (getDirection() == 180)
        move = -2;

    if (getWorld()->attemptMove(getX()+move, getY()))
        moveTo(getX()+move, getY());
    else
        setDead();
}

bool Projectile::shouldDamage(int x, int y)
{
    return getWorld()->overlapsEnemy(x, y);
}

bool PiraBall::shouldDamage(int x, int y)
{
    return getWorld()->overlapsPeach(x, y);
}

void Projectile::damageSomething(int x, int y)
{
    getWorld()->damageEnemy(x, y);
}

void PiraBall::damageSomething(int x, int y)
{
    getWorld()->damagePeach();
}

/////////////////////////////////////////////////////
// -------------------- OBJECT ----------------------
/////////////////////////////////////////////////////

void Block::bonk()
{
    if (m_goodie == BLOCK_NONE)
        getWorld()->playSound(SOUND_PLAYER_BONK);
    else
    {
        getWorld()->playSound(SOUND_POWERUP_APPEARS);
        if (m_goodie == BLOCK_MUSHROOM)
            getWorld()->addMushroom(getX()/SPRITE_WIDTH, (getY()+8)/SPRITE_WIDTH);
        else if (m_goodie == BLOCK_STAR)
            getWorld()->addStar(getX()/SPRITE_WIDTH, (getY()+8)/SPRITE_WIDTH);
        else
            getWorld()->addFlower(getX()/SPRITE_WIDTH, (getY()+8)/SPRITE_WIDTH);
        m_goodie = BLOCK_NONE;
    }
}

void LevelChange::doSomething()
{
    if(!isAlive())
        return;
    if (getWorld()->overlapsPeach(getX(), getY()))
    {
        getWorld()->increaseScore(1000);
        setDead();
        levelCompleted();
    }
}

void Flag::levelCompleted()
{
    getWorld()->flagReached();
}

void Mario::levelCompleted()
{
    getWorld()->marioReached();
}

///////////////////////////////////////////////////
// -------------------- GOODIE --------------------
///////////////////////////////////////////////////

void Goodie::doSomething()
{
    if (getWorld()->overlapsPeach(getX(), getY()))
    {
        goodieConsumed();
        setDead();
        getWorld()->playSound(SOUND_PLAYER_POWERUP);
    } else if (!getWorld()->hasSolidBelow(getX(), getY(), 2))
        moveTo(getX(), getY()-2);
    
    int move = 2;
    if (getDirection() == 180)
        move = -2;
    
    if (getWorld()->attemptMove(getX()+move, getY()))
        moveTo(getX()+move, getY());
    else
        move == 2 ? setDirection(180) : setDirection(0);
}

void Flower::goodieConsumed()
{
    getWorld()->increaseScore(50);
    getWorld()->setShoot(true);
    getWorld()->setHealth(2);
}

void Mushroom::goodieConsumed()
{
    getWorld()->increaseScore(75);
    getWorld()->setJump(true);
    getWorld()->setHealth(2);
}

void Star::goodieConsumed()
{
    getWorld()->increaseScore(100);
    getWorld()->setStar(true);
    getWorld()->setInvincible(150);
}
