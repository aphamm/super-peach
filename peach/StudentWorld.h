#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>

class StudentWorld : public GameWorld
{
public:
    
    // essential
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // location
    bool overlapsPeach(int x, int y);
    bool overlapsEnemy(int x, int y);
    bool attemptMove(int x, int y);
    void bonkActors(int x, int y);
    void bonkPeach(int x, int y);
    bool hasSolidBelow(int x, int y, int d);
    bool wontFall(int x, int y, int d);
    
    // piranha
    bool peachLevel(int y);
    int peachDistance(int x);
    
    // damage
    void damageEnemy(int x, int y);
    void damagePeach();
    
    // mutators
    void setStar(bool state) { m_peach->setStar(state); };
    void setShoot(bool state) { m_peach->setShoot(state); };
    void setJump(bool state) { m_peach->setJump(state); };
    void setInvincible(int ticks) { m_peach->setInvincible(ticks); };
    void setHealth(int health) { m_peach->setHealth(health); };
    void flagReached() { m_flag = true; };
    void marioReached() { m_mario = true; };
    
    // add actor
    void addStar(int x, int y) { m_actors.push_back(new Star(this, x, y)); };
    void addFlower(int x, int y) { m_actors.push_back(new Flower(this, x, y)); };
    void addMushroom(int x, int y) { m_actors.push_back(new Mushroom(this, x, y)); };
    void addShell(int x, int y, int dir) { m_actors.push_back(new Shell(this, x, y, dir)); };
    void addPeachBall(int x, int y, int dir) { m_actors.push_back(new PeachBall(this, x, y, dir)); };
    void addPiraBall(int x, int y, int dir) { m_actors.push_back(new PiraBall(this, x, y, dir)); };
    
    // accessor
    bool hasStar() { return m_peach->hasStar(); };
    
private:
    std::vector<Actor*> m_actors;
    Peach* m_peach;
    bool m_flag;
    bool m_mario;
    bool overlaps(int x1, int y1, int x2, int y2);
};

#endif // STUDENTWORLD_H_


