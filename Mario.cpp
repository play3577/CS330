/* 
 *  Mario.cpp
 *  Mario
 *
 *  Created by Andrew Peddicord on 2/1/11.
 *  Copyright 2011 Capital University. All rights reserved.
 *
 * filed edited by Eric, Nate and Drew
 */


#include "Mario.h"
#include "LListIterator.h"
#include "Level.h"
#include "Breakable.h"
#include "Nonbreakable.h"
#include "Goomba.h"
#include "Shell.h"
#include "MarioFireball.h"
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

//------------------------------------------------------------
void Mario::draw(bool update)
{
    if (!this->isDead()){
        //Determine power up
        int dState= 0;
        
        if (this->state_ == BIG_STATE) {
            dState = 1;
        }
        else if ( this->state_ == FIRE_STATE){
            dState = 2;
        }
        if (update) {
            
        
    
    
    //Determine sprite possition
    if (this->getYVelocity() != 0.0) {
        texturePos = 3;
    }
    else if (this->getXVelocity() != 0.0){
        if (texturePos == 1) {
            texturePos = 2;
        }
        else{
            texturePos = 1;
        }
    }
    else{
        texturePos = 0;
    }
    }

    //Set proper blending for alpha        
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texture_[dState][texturePos]);
    }
    else{
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, deadtexture_);
    }
    
    //Draw Quad
    glBegin( GL_QUADS );
    glColor4f(0.7f,0.9f,1.0f,1.0f);
    if (this->direction_ == 1) {
        glTexCoord2d(0.0,0.0); glVertex2d(left(),bottom());
        glTexCoord2d(1.0,0.0); glVertex2d(right(),bottom());
        glTexCoord2d(1.0,1.0); glVertex2d(right(),top());
        glTexCoord2d(0.0,1.0); glVertex2d(left(),top());
    }
    else{
        glTexCoord2d(0.0,0.0); glVertex2d(right(),bottom());
        glTexCoord2d(1.0,0.0); glVertex2d(left(),bottom());
        glTexCoord2d(1.0,1.0); glVertex2d(left(),top());
        glTexCoord2d(0.0,1.0); glVertex2d(right(),top());
    }
    glEnd();
    
    //Disable unwanted gl modes
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
//------------------------------------------------------------
//constructor for Mario Class
Mario::Mario()
{
    //init the private instance variables to default value
    state_ = SMALL_STATE;
    jumpCount_ = 0;
    starCount_ = 0;
    hitCount_ = 0;
    direction_ = 1;
    
    isDead_ = false;
    isInvincible_ = false;
    compleateLevel_ = false;
    
    //init the keys
    jumpKey_ = false;
    rightKey_ = false;
    leftKey_ = false;
    sprintKey_ = false;
    fireballKey_ = false;
    
    //Set X and Y velocity
    this->setXVelocity(0.0);
    this->setYVelocity(0.0);
    
        
    sprite();
}
//------------------------------------------------------------
//updates Mario's movement info when a button is pushed
void Mario::updateKeyDown(unsigned char button)
{
    // if the key to move left is pushed
    // set Mario's direction and X velocity
    // to move left
    if (button == 'a')
    {
        leftKey_ = true;
        direction_ = 0;
        //set Mario's Velocity
        if (sprintKey_ == true)
        {
            this->setXVelocity(-2.0);
        }
        else 
        {
            this->setXVelocity(-1.0);
        }
    }
    // if the key to move right is pushed
    // set Mario's direction and X velocity
    // to move right
    if (button == 'd')
    {
        rightKey_ = true;
        direction_ = 1;
        //set Mario's velocity
        if (sprintKey_ == true)
        {
            this->setXVelocity(2.0);
        }
        else 
        {
            this->setXVelocity(1.0);
        }
    }
    // if the jump key is pushed and Mario
    // is not already jumping, set jump count
    // and Mario's Y velocity
    if (button == 'w')
    {
        jumpKey_ = true;
        if (jumpCount_ == 0 && this->getYVelocity() == 0.0) 
        {
            jumpCount_ = 50;
            
            //set Mario's velocity
            this->setYVelocity(2.0);
        }
    }
    // if the sprint/fireball key is pushed,
    // and if Mario is in FIRE_STATE, create
    // a MarioFireball and set coordinates
    // and velocity
    if (button == 'j')
    {
        sprintKey_ = true;
        if (this->state_ == FIRE_STATE) {
            MarioFireball *fb = new MarioFireball;
            fb->setTop(this->top());
            fb->setBottom(this->top() - 8);
            if (this->direction_ == 0) {
                fb->setLeft(this->left());
                fb->setRight(this->left() - 8);
            } else {
                fb->setLeft(this->right());
                fb->setRight(this->right() + 8);
            }
            if (direction_ == 1) {
                fb->setXVelocity(1.0);
            }
            else {
                fb->setXVelocity(-1.0);
            }
            fb->setYVelocity(-0.5);
            Level::sharedLevel()->addMovable(fb);
        }
    }
}
//------------------------------------------------------------
//upadates Mario's info when a button is let up
void Mario::updateKeyUp(unsigned char button)
{
    // if the left key is let up
    // stop Mario from moving and
    // move right if the right key
    // is being pushed
	if (button == 'a') {
		leftKey_ = false;
        if (rightKey_) {
            if (this->getXVelocity() < 0) {
                this->setXVelocity(1.0);
            }
        }
        else {
            this->setXVelocity(0.0);
        }
	}
    // if the jump key is let up,
    // Mario falls
	if (button == 'w') {
		jumpKey_ = false;
        if (jumpCount_ > 15) {
            jumpCount_ = 15;
        } else {
            jumpCount_ = 0;
            this->setYVelocity(-2.0);
        }
	}
    // if the right key is let up
    // stop Mario from moving and
    // move left if the left key
    // is being pushed
	if (button == 'd') {
		rightKey_ = false;
        if (leftKey_) {
            if (this->getXVelocity() > 0) {
                this->setXVelocity(-1.0);
            }
        }
        else {
            this->setXVelocity(0.0);
        }
	}
    // stop sprint
	if (button == 'j') {
		sprintKey_ = false;
	}
}
//------------------------------------------------------------
//method to calculate Marios movement
void Mario::move()
{
    //actually does the movement of Mario
    this->setRight(this->right() + this->getXVelocity());
    this->setLeft(this->left() + this->getXVelocity());
    this->setTop(this->top() + this->getYVelocity());
    this->setBottom(this->bottom() + this->getYVelocity());
}
//------------------------------------------------------------
//Handels all jump cases
void Mario::jump() {
    if (this->jumpCount_ > 0) {
        jumpCount_--;
    } else if (this->getYVelocity() > 0.0) {
        this->setYVelocity(-2.0);
    }
}
//------------------------------------------------------------
//updates Mario for one Frame
void Mario::updateScene()
{
    //depending on whats happening we need to update Mario
    // check the key and move mario according to the button that is being pushed
    //call the check methods to check to see if Mario is running into things
    //this works one frame at a time
    //if Mario jumps decrease the jumpCount_ by 1 every frame
    if (!isDead()) {
        check();
        jump();
        move();
        if (starCount_ > 0)
            starCount_ --;
        if (hitCount_ > 0)
            hitCount_--;
    }
    
}
//------------------------------------------------------------
//Creates a fireball
bool Mario::fireball()
{
	return false;
}
//------------------------------------------------------------
void Mario::setLeftBound(int leftBound)
{
    leftBound_ = leftBound;
}
//-------------------------------------------------------------
//method that calculate the intersections of Mario and objects
//to see if Mario runs into anything
void Mario::check() {
    Drawable *objb, *objt, *objl, *objr;

    objt = this->checkTop();
    //All items that can hit Mario from the top
    if (objt)
        switch (objt->objectType()) {
            // if the block is Nonbreakable, stop Mario
            case OFFQUESTION:
            case REGULAR:
                if (this->getYVelocity() > 0) {
                    this->setXVelocity(0.0);
                    this->jumpCount_ = 0;
                }
                break;
            // if the block is question, stop Mario and 
            // generate reward based on Mario's state
            case QUESTION:
                if (this->getYVelocity() > 0) {
                    this->setXVelocity(0.0);
                    this->jumpCount_ = 0;
                }
                if (this->getYVelocity() >= 0) {
                    ((Nonbreakable*)objt)->generateReward(this->getState() != SMALL_STATE);
                }
                break;
            // if the block is a breakable block, stop Mario,
            // check if Mario can break it, and then check if
            // there is an enemy on top of the block and kill
            // the enemy
            case BREAKABLE:
                if (this->getYVelocity() > 0) {
                    this->setXVelocity(0.0);
                    this->jumpCount_ = 0;
                }
                if (this->getYVelocity() >= 0) {
                    ((Breakable*) objt)->breakBlock(this->getState() != SMALL_STATE);
                    (game)->breakBlock(this->getState() != SMALL_STATE);
                    
                    if (this->getState() != SMALL_STATE)
                    {
                        this->setTop(this->top() + 16);
                        objt = this->checkTop();
                        if (objt)
                            Level::sharedLevel()->removeDrawable(objt);
                        
                        this->setTop(this->top() - 16);
                    }
                }
                break;
            // if an enemy falls on top of Mario, 
            // SMALL_STATE = Mario dies
            // if starCount_ > 0, the enemy dies
            // otherwise decrease state and make
            // Mario invincible for 200 frames
            case GOOMBA:
            case SHELL:
            case ENEMYFIREBALL:
            case TURTLE:
                if (state_ > SMALL_STATE)
                {
                    if (hitCount_ == 0){
                        hitCount_ = 200;
                        if (state_ == BIG_STATE)
                        {
                            this->setTop(this->top() - 8.0);
                            state_ = SMALL_STATE;
                        }
                        else if (state_ == FIRE_STATE)
                        {
                            state_ = BIG_STATE;
                        }
                    }
                }
                if (starCount_ > 0)
                {
                    game->jumpEnemy(1);
                    Level::sharedLevel()->removeDrawable(objr);
                }
                else if (starCount_ == 0 && hitCount_ == 0)
                {
                    this->isDead_ = true;
                }
                break;
            // if a reward falls on top of Mario or he jumps
            // into a reward, increase state accordingly or
            // add points
            case MUSHROOM:
                game->addPowerup();
                Level::sharedLevel()->removeDrawable(objt);
                if (this->state_ == SMALL_STATE) {
                    this->state_ = BIG_STATE;
                    this->setTop(this->top() + 8);
                }
                break;
            // Make Mario invincible for 1000 frames
            case STAR:
                game->addPowerup();
                starCount_ = 1000;
                break;
            case FIREFLOWER:
                game->addPowerup();
                if (state_ == SMALL_STATE)
                {
                    this->setTop(this->top() + 8);
                }
                this->state_ = FIRE_STATE;
                Level::sharedLevel()->removeDrawable(objt);
                break;
            case COIN:
                game->addCoin();
                Level::sharedLevel()->removeDrawable(objt);
                break;
                
        }
    objb = this->checkBottom();
    //All objects that can hit Mario from the bottom
    if (objb) {
        Shell *nshell;
        switch (objb->objectType()) {
            // if Mario lands on anything solid
            // stop Mario from falling
            case PIPE:
            case OFFQUESTION:
            case BREAKABLE:
            case REGULAR:
            case QUESTION:
                if (this->getYVelocity() < 0) {
                    this->setYVelocity(0.0);
                }
                break;
            // Mario lands on an enemy
            // if Turtle, generate shell
            case TURTLE:
                nshell = new Shell();
                nshell->setTop(objb->top()-8);
                nshell->setRight(objb->right());
                nshell->setLeft(objb->left());
                nshell->setBottom(objb->bottom());
                Level::sharedLevel()->addMovable(nshell);
                //break;
            // if Goomba, kill enemy and 
            // reset jumpCount to half for bounce
            case GOOMBA:
                game->jumpEnemy(1);
                Level::sharedLevel()->removeDrawable(objb);
                this->jumpCount_ = 25;
                this->setYVelocity(2.0);
            case SHELL:
            case ENEMYFIREBALL:
                break;
            // if Mario lands on a reward, increase state accordingly or
            // add points
            case MUSHROOM:
                game->addPowerup();
                Level::sharedLevel()->removeDrawable(objb);
                if (this->state_ == SMALL_STATE) {
                    this->state_ = BIG_STATE;
                    this->setTop(this->top() + 8);
                }
                break;
            // Make Mario invincible for 1000 frames
            case STAR:
                game->addPowerup();
                starCount_ = 1000;
                break;
            case FIREFLOWER:
                game->addPowerup();
                if (state_ == SMALL_STATE)
                {
                    this->setTop(this->top() + 8);
                }
                this->state_ = FIRE_STATE;
                Level::sharedLevel()->removeDrawable(objb);
                break;
            case COIN:
                game->addCoin();
                Level::sharedLevel()->removeDrawable(objb);
                break;
            // Mario has reached the end of the level
            case FLAG:
                game->touchFlag(this->bottom());
                compleateLevel_=true;
        }
    } else {
        if (this->getYVelocity() == 0.0) {
            this->setYVelocity(-2.0);
        }
    }
    objl = this->checkLeft();
    //All objects that can hit Mario from the left
    if (objl) {
        switch (objl->objectType()) {
            // if Mario runs into anything solid
            // stop Mario from moving through object
            case PIPE:
            case BREAKABLE:
            case REGULAR:
            case OFFQUESTION:
            case QUESTION:
                if (this->getXVelocity() < 0) {
                    this->setXVelocity(0.0);
                }
                break;
            // if Mario runs into an enemy, 
            // SMALL_STATE = Mario dies
            // if starCount_ > 0, the enemy dies
            // otherwise decrease state and make
            // Mario invincible for 200 frames
            case GOOMBA:
            case SHELL:
            case ENEMYFIREBALL:
            case TURTLE:
                if (state_ > SMALL_STATE)
                {
                    if (hitCount_ == 0){
                        hitCount_ = 200;
                        if (state_ == BIG_STATE)
                        {
                            this->setTop(this->top() - 8.0);
                            state_ = SMALL_STATE;
                        }
                        else if (state_ == FIRE_STATE)
                        {
                            state_ = BIG_STATE;
                        }
                    }
                }
                if (starCount_ > 0)
                {
                    game->jumpEnemy(1);
                    Level::sharedLevel()->removeDrawable(objr);
                }
                else if (starCount_ == 0 && hitCount_ == 0)
                {
                    this->isDead_ = true;
                }
                break;
            // Mario runs into a reward, update state accordingly
            // or add points
            case MUSHROOM:
                game->addPowerup();
                Level::sharedLevel()->removeDrawable(objl);
                if (this->state_ == SMALL_STATE) {
                    this->state_ = BIG_STATE;
                    this->setTop(this->top() + 8);
                }
                break;
            // Make Mario invincible for 1000 frames
            case STAR:
                game->addPowerup();
                starCount_ = 1000;
                break;
            case FIREFLOWER:
                game->addPowerup();
                if (state_ == SMALL_STATE)
                {
                    this->setTop(this->top() + 8);
                }
                this->state_ = FIRE_STATE;
                Level::sharedLevel()->removeDrawable(objl);
                break;
            case COIN:
                game->addCoin();
                Level::sharedLevel()->removeDrawable(objl);
                break;
            // Mario has reached the end of the level
            case FLAG:
                game->touchFlag(this->bottom());
                compleateLevel_=true;
                break;
        }
    } 
    else if (leftKey_) {
        if (sprintKey_) {
            this->setXVelocity(-1.2);
        } else {
            this->setXVelocity(-1.0);
        }
    }
    objr = this->checkRight();
    //All objects that can hit Mario from the right
    if (objr) {
        switch (objr->objectType()) {
            // if Mario runs into anything solid
            // stop Mario from moving through object
            case PIPE:
            case OFFQUESTION:
            case BREAKABLE:
            case REGULAR:
            case QUESTION:
                if (this->getXVelocity() > 0) {
                    this->setXVelocity(0.0);
                }
                break;
            // if Mario runs into an enemy, 
            // SMALL_STATE = Mario dies
            // if starCount_ > 0, the enemy dies
            // otherwise decrease state and make
            // Mario invincible for 200 frames
            case GOOMBA:
            case SHELL:
            case ENEMYFIREBALL:
            case TURTLE:
                if (state_ > SMALL_STATE)
                {
                    if (hitCount_ == 0){
                        hitCount_ = 200;
                        if (state_ == BIG_STATE)
                        {
                            this->setTop(this->top() - 8.0);
                            state_ = SMALL_STATE;
                        }
                        else if (state_ == FIRE_STATE)
                        {
                            state_ = BIG_STATE;
                        }
                    }
                }
                if (starCount_ > 0)
                {
                    game->jumpEnemy(1);
                    Level::sharedLevel()->removeDrawable(objr);
                }
                else if (starCount_ == 0 && hitCount_ == 0)
                {
                    this->isDead_ = true;
                }
                break;
            // Mario runs into a reward, update state accordingly
            // or add points
            case MUSHROOM:
                game->addPowerup();
                Level::sharedLevel()->removeDrawable(objr);
                if (this->state_ == SMALL_STATE) {
                    this->state_ = BIG_STATE;
                    this->setTop(this->top() + 8);
                }
                break;
            // Make Mario invincible for 1000 frames
            case STAR:
                game->addPowerup();
                starCount_ = 50;
                break;
            case FIREFLOWER:
                game->addPowerup();
                if (state_ == SMALL_STATE)
                {
                    this->setTop(this->top() + 8);
                }
                this->state_ = FIRE_STATE;
                Level::sharedLevel()->removeDrawable(objr);
                break;
            case COIN:
                game->addCoin();
                Level::sharedLevel()->removeDrawable(objr);
                break;
            // Mario has reached the end of the level
            case FLAG:
                game->touchFlag(this->bottom());
                compleateLevel_=true;
                break;
                
        }
    } 
    else if (rightKey_) {
        if (sprintKey_) {
            this->setXVelocity(1.2);
        } else {
            this->setXVelocity(1.0);
        }
    }
    //stops Mario moving out of the left bound
    if (this->left() < leftBound_ && this->getXVelocity() < 0)
    {
        this->setXVelocity(0.0);
    }
    // if Mario falls off a ledge or if time runs out
    // Mario dies
    if (this->top() <= 0 || game->getTime() <= 0) {
        this->isDead_ = true;
    }
}
//------------------------------------------------------------
void Mario::sprite()
{
    texturePos = 0;
    
    // Mac environment variable for home directory
    char *cHomeDir = NULL;
    
    cHomeDir = getenv("HOME");
    
    // I think Windows uses HOMEPATH
    if (!cHomeDir) {
        cHomeDir = getenv("HOMEPATH");
    }
    
    // Set sprite home
    string homeDir = cHomeDir;
    string iName, jName;
    homeDir += "/CS330/sprites/";
    
    //Generate Sprite filenames
    string pos;
    int height = 32;
    
    for (int j = 0; j<=2; ++j) {
        stringstream out0;
        //Generates Filename
        jName = homeDir;
        out0<<j;
        pos = out0.str();
        jName+=pos;
        jName+="mario";
        
        if (j != 0) {
            height = 64;
        }
        
        
        
        for (int i = 0; i<=3; ++i) {
            stringstream out1;
            //Generates Filename
            iName = jName;
            out1<<i;
            pos = out1.str();
            iName += pos;
            iName += ".tex";
            
            //Load Sprite into array of textures
            FILE *fp = fopen(iName.c_str(), "r");
            unsigned char *texture = new unsigned char[4 * 32 * height];
            if (fread(texture, sizeof(unsigned char), 4 * 32 * height, fp)
                != 4* 32 *height) {
                fprintf(stderr, "error reading %s", iName.c_str());
            }
            fclose(fp);
            
            //Bind Texture to a GLuint
            glGenTextures(1, &texture_[j][i]);
            glBindTexture(GL_TEXTURE_2D, texture_[j][i]);
            
            //Set parameters for how the texture is displayed
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );        
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_NEAREST );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                            GL_LINEAR );        
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                            GL_CLAMP );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                            GL_CLAMP );
            
            //Build Mipmap
            gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 32, height, GL_RGBA,
                              GL_UNSIGNED_BYTE, texture);
            delete [] texture;
            
        }
    }
    
    iName = homeDir+"3mario0.tex";
    //Read in the texture file
    FILE *fp = fopen(iName.c_str(), "r");
    unsigned char *texture = new unsigned char[4 * 32 * 32];
    if (fread(texture, sizeof(unsigned char), 4 * 32 * 32, fp)
        != 4* 32 *32) {
        fprintf(stderr, "error reading %s", iName.c_str());
    }
    fclose(fp);
    
    //Bind Texture to a GLuint
    glGenTextures(1, &deadtexture_);
    glBindTexture(GL_TEXTURE_2D, deadtexture_);
    
    //Set parameters for how the texture is displayed
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );        
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR );        
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    GL_CLAMP );
    
    //Build Mipmap
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 32, 32, GL_RGBA,
                      GL_UNSIGNED_BYTE, texture);
    delete [] texture;

}
//------------------------------------------------------------
void Mario::reset(){
    if (isDead()) {
        state_ = SMALL_STATE;
    }
    jumpCount_ = 0;
    starCount_ = 0;
    
    isDead_ = false;
    isInvincible_ = false;
    compleateLevel_ = false;
    
    //init the keys
    jumpKey_ = false;
    rightKey_ = false;
    leftKey_ = false;
    sprintKey_ = false;
    fireballKey_ = false;
	direction_ = 1;
    
    //Set X and Y velocity
    this->setXVelocity(0.0);
    this->setYVelocity(0.0);
}
