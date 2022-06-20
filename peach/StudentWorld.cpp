#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

////////////////////////////////////////////////////////
// -------------------- ESSENTIAL ----------------------
////////////////////////////////////////////////////////

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
 : GameWorld(assetPath), m_flag(false), m_mario(false)
{
    m_actors.clear();
    m_peach = nullptr;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    m_flag = false;
    m_mario = false;
    
    string level;
    if (getLevel() < 10)
        level = "0" + to_string(getLevel());
    else if (getLevel() >= 10)
        level = to_string(getLevel());
    else
        return GWSTATUS_LEVEL_ERROR;
    
    Level lev(assetPath());
    string level_file = "level" + level + ".txt";
    Level::LoadResult result = lev.loadLevel(level_file);
    
    if (result == Level::load_fail_file_not_found)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        Level::GridEntry ge;
                
        for (int x = 0; x < 32; x++)
        {
            for (int y = 0; y < 32; y++)
            {
                ge = lev.getContentsOf(x, y);
                switch (ge)
                {
                    case Level::peach:
                        m_peach = new Peach(this, x, y);
                        m_actors.push_back(m_peach);
                        break;
                    case Level::koopa:
                        m_actors.push_back(new Koop(this, x, y));
                        break;
                    case Level::goomba:
                        m_actors.push_back(new Goom(this, x, y));
                        break;
                    case Level::piranha:
                        m_actors.push_back(new Pira(this, x, y));
                        break;
                    case Level::block:
                        m_actors.push_back(new Block(this, x, y, BLOCK_NONE));
                        break;
                    case Level::star_goodie_block:
                        m_actors.push_back(new Block(this, x, y, BLOCK_STAR));
                        break;
                    case Level::mushroom_goodie_block:
                        m_actors.push_back(new Block(this, x, y, BLOCK_MUSHROOM));
                        break;
                    case Level::flower_goodie_block:
                        m_actors.push_back(new Block(this, x, y, BLOCK_FLOWER));
                        break;
                    case Level::pipe:
                        m_actors.push_back(new Pipe(this, x, y));
                        break;
                    case Level::flag:
                        m_actors.push_back(new Flag(this, x, y));
                        break;
                    case Level::mario:
                        m_actors.push_back(new Mario(this, x, y));
                        break;
                    case Level::empty:
                        break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    for (vector<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
    {
        if ((*p)->isAlive())
            (*p)->doSomething();
        
        if (!m_peach->isAlive())
        {
            decLives();
            playSound(SOUND_PLAYER_DIE);
            return GWSTATUS_PLAYER_DIED;
        }
            
        if (m_mario) // reach mario
        {
            playSound(SOUND_GAME_OVER);
            return GWSTATUS_PLAYER_WON;
        }
        
        if (m_flag) // completed level
        {
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    // remove dead actors
    vector<Actor*>::iterator p = m_actors.begin();
    for ( ; p != m_actors.end() ; )
    {
        if (!(*p)->isAlive())
        {
            delete *p;
            m_actors.erase(p);
        } else
            p++;
    }
    
    // game status
    ostringstream oss;
    oss.fill('0');
    oss << "Lives: " << getLives() << "  ";
    oss << "Level: " << getLevel() << "  ";
    oss << "Points: " << getScore();
    if (m_peach->hasStar())
        oss << " StarPower!";
    if (m_peach->hasShoot())
        oss << " ShootPower!";
    if (m_peach->hasJump())
        oss << " JumpPower!";
    setGameStatText(oss.str());
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // delete m_peach;
    m_peach = nullptr;
    
    vector<Actor*>::iterator p = m_actors.begin();
    while (p != m_actors.end())
    {
        delete *p;
        p = m_actors.erase(p);
    }
}

///////////////////////////////////////////////////////
// -------------------- LOCATION ----------------------
///////////////////////////////////////////////////////

bool StudentWorld::overlaps(int x1, int y1, int x2, int y2)
{
    if (x1 + SPRITE_WIDTH - 1 > x2 && x1 < x2 + SPRITE_WIDTH - 1)
        if (y1 + SPRITE_HEIGHT - 1 > y2 && y1 < y2 + SPRITE_HEIGHT - 1)
            return true;
    return false;
}

bool StudentWorld::overlapsPeach(int x, int y)
{
    if (overlaps(x, y, m_peach->getX(), m_peach->getY()))
        return true;
    return false;
}

bool StudentWorld::overlapsEnemy(int x, int y)
{
    for (vector<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
        if ((*p)->isDamageable() && *p != m_peach && (*p)->isAlive())
            if (overlaps(x, y, (*p)->getX(), (*p)->getY()))
                return true;
    return false;
}

bool StudentWorld::attemptMove(int x, int y)
{
    for (vector<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
        if ((*p)->blockActor() && overlaps(x, y, (*p)->getX(), (*p)->getY()))
                return false;
    return true;
}

void StudentWorld::bonkActors(int x, int y)
{
    for (vector<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
        if ((*p)->isAlive() && *p != m_peach && overlaps(x, y, (*p)->getX(), (*p)->getY()))
                (*p)->bonk();
}

void StudentWorld::bonkPeach(int x, int y)
{
    for (vector<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
        if ((*p)->isAlive() && (*p)->isLethal() && overlaps(x, y, (*p)->getX(), (*p)->getY()))
                m_peach->bonk();
}

bool StudentWorld::hasSolidBelow(int x, int y, int d)
{
    for (vector<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
        if ((*p)->blockActor())
            if (x + SPRITE_WIDTH - 1 > (*p)->getX() && x < (*p)->getX() + SPRITE_WIDTH - 1)
                if (y >= (*p)->getY() && y <= (*p)->getY() + SPRITE_HEIGHT - 1 + d)
                    return true;
    return false;
}

bool StudentWorld::wontFall(int x, int y, int d)
{
    if (d == 0)
    {
        if (hasSolidBelow(x, y, 1) && hasSolidBelow(x + SPRITE_WIDTH, y, 1))
            return true;
    } else {
        if (hasSolidBelow(x - SPRITE_WIDTH, y, 1) && hasSolidBelow(x, y, 1))
            return true;
    }
    return false;
}

//////////////////////////////////////////////////////
// -------------------- PIRANHA ----------------------
//////////////////////////////////////////////////////

bool StudentWorld::peachLevel(int y)
{
    if ( m_peach->getY() + SPRITE_HEIGHT > y - 1.5 * SPRITE_HEIGHT &&
        m_peach->getY() < y + 1.5 * SPRITE_HEIGHT)
        return true;
    return false;
}

int StudentWorld::peachDistance(int x)
{
    return m_peach->getX() - x;
}

/////////////////////////////////////////////////////
// -------------------- DAMAGE ----------------------
/////////////////////////////////////////////////////

void StudentWorld::damageEnemy(int x, int y)
{
    for (vector<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
        if ((*p)->isDamageable() && *p != m_peach)
            if (overlaps(x, y, (*p)->getX(), (*p)->getY()))
                (*p)->getDamaged();
}

void StudentWorld::damagePeach()
{
    m_peach->bonk();
}
