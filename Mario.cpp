/* 
 *  Mario.cpp
 *  Mario
 *
 *  Created by Andrew Peddicord on 2/1/11.
 *  Copyright 2011 Capital University. All rights reserved.
 *
 * filed edited by Drew and Nate
 */


#include <cstdio>
#include "Mario.h"
#include "LListIterator.h"
#include "Level.h"
#include "Breakable.h"
#include "Nonbreakable.h"
#include <iostream>

using std::cout;
using std::endl;


GLuint Mario::textureNum = -1;

//------------------------------------------------------------
void Mario::draw()
{
	//glColor3ub(255, 0, 0);
	int i;
	
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureNum);
    glBegin(GL_POLYGON);
    glTexCoord2d(0, 0);
    glVertex2d(left(), bottom());
    glTexCoord2d(0, 1);
    glVertex2d(left(), top());
    
    //glVertex2d(((right()-left())/2)+left(), top());
    glTexCoord2d(1,1);
    glVertex2d(right(),top());
    glTexCoord2d(1, 0);
    glVertex2d(right(), bottom());
    glEnd();
    glDisable(GL_TEXTURE_2D);
	
}

void Mario::loadTexture()
{
    if (textureNum == -1) {
        FILE *fp = fopen("/Users/dreed/Downloads/wyndy.tex", "r");
        unsigned char *texture = new unsigned char[4 * 256 * 256];
        if (fread(texture, sizeof(unsigned char), 4 * 256 * 256, fp) != 4* 256 *256) {
            fprintf(stderr, "error reading wyndy.tex");
        }
        fclose(fp);
        glGenTextures(1, &textureNum);
        glBindTexture(GL_TEXTURE_2D, textureNum);
        
        // select modulate to mix texture with color for shading
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
        
        // when texture area is small, bilinear filter the closest mipmap
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_NEAREST );
        // when texture area is large, bilinear filter the first mipmap
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        
        // if wrap is true, the texture wraps over at the edges (repeat)
        //       ... false, the texture ends at the edges (clamp)
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        GL_CLAMP );
        
        // build our texture mipmaps
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, texture);
        delete [] texture;
    }
}

//------------------------------------------------------------
//constructor for Mario Class
Mario::Mario()
{
    //init the private instance variables to default value
    state_ = SMALL_STATE;
    jumpCount_ = 0;
    starCount_ = 0;
    
    isInvincible_ = false;
    
    //init the keys
    jumpKey_ = false;
    rightKey_ = false;
    leftKey_ = false;
    sprintKey_ = false;
    fireballKey_ = false;
    
    //Set X and Y velocity
    this->setXVelocity(0.0);
    this->setYVelocity(0.0);
}
//------------------------------------------------------------
//updates Mario's movement info when a button is pushed
void Mario::updateKeyDown(unsigned char button)
{
    if (button == 'a')
    {
        leftKey_ = true;
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
    
    else if (button == 'd')
    {
        rightKey_ = true;
        
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
    
    if (button == 'j')
    {
        sprintKey_ = true;
    }
    
    if (button == 'k')
    {
        fireballKey_ = true;
    }
}
//------------------------------------------------------------
//upadates Mario's info when a button is let up
void Mario::updateKeyUp(unsigned char button)
{
	if (button == 'a') {
		leftKey_ = false;
        if (rightKey_) {
            if (this->getXVelocity() < 0) {
                this->setXVelocity(1.0);
            }
            else {
                this->setXVelocity(0.0);
            }
        }
        else {
            this->setXVelocity(0.0);
        }
	}
	if (button == 'w') {
		jumpKey_ = false;
        if (jumpCount_ > 15) {
            jumpCount_ = 15;
        } else {
            jumpCount_ = 0;
            this->setYVelocity(-2.0);
        }
	}
    
	if (button == 'd') {
		rightKey_ = false;
        if (leftKey_) {
            if (this->getXVelocity() > 0) {
                this->setXVelocity(-1.0);
            }
            else {
                this->setXVelocity(0.0);
            }
        }
        else {
            this->setXVelocity(0.0);
        }
	}
	if (button == 'j') {
		sprintKey_ = false;
	}
	if (button == 'k') {
		fireballKey_ = false;
	}
}
//------------------------------------------------------------
//method to calculate Marios movement
void Mario::move()
{
    //actually does the movement of Mario
    this->setRight(this->right() + this->getXVelocity());
    this->setLeft(this->left() + this->getXVelocity());
}
//------------------------------------------------------------
//Handels all jump cases
void Mario::jump() {
    Drawable *node;
    int object;
    if (jumpCount_ > 0) {
        node = this->checkAbove();
        //There is a object above Mario
        if (node) {
            object = node->objectType();
            if (object == -1) { //Will be fixed by AllMovable Group
                //this->setTop(this->top() + this->getYVelocity());
                //this->setBottom(this->bottom() + this->getYVelocity());
            } else if (object == breakable_){
                Breakable *temp = (Breakable*)node;
                temp->breakBlock(state_ == BIG_STATE || state_ == FIRE_STATE);
                jumpCount_ = 1;
            } else if (object == question_) {
                Nonbreakable *temp = (Nonbreakable*)node;
                temp->generateReward(true); //Question for the Nonbreakable group
                jumpCount_ = 1;
            }
        } else {
            //There is no block above Mario
            this->setTop(this->top() + this->getYVelocity());
            this->setBottom(this->bottom() + this->getYVelocity());
        }
        jumpCount_--;
        //Set Mario's Y Velocity
        if (jumpCount_ == 0) {
            this->setYVelocity(-2.0);
        }
    } else if (this->getYVelocity() < 0) {
        node = this->checkBelow();
        //There is an object below Mario, so stop him from moving
        if (node) {
            object = node->objectType();
            if (object == -1) {
                //this->setBottom(this->bottom() + this->getYVelocity());
            } else {
                this->setYVelocity(0.0);
            }
        } else {
            //No block below Mario
            this->setBottom(this->bottom() + this->getYVelocity());
            this->setTop(this->top() + this->getYVelocity());
        }
    } else {
        node = this->checkBelow();
        if (!node) {
            this->setYVelocity(-2.0);
        }
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
    
    //check();
    move();
    jump();
}
//------------------------------------------------------------
//method that calculate the intersections of Mario and objects
//to see if Mario runs into anything
bool Mario::check()
{
    //get all the values of the objects so Mario knows what to do 
    // the object
    //check each case for each object
    //object list
    //flag: 1, breakable: 2, nonbreakable: 3, Fireflower: 4, coin: 5
    //Mario: 6, Goomba: 7, Mushroom: 8, Plant: 9, Shell: 10, Star: 11
    //Turtle: 12, EnemyFireball: 13, MarioFireball: 14
    
    //the level 
    Level *level = Level::sharedLevel();
    
    //the lists of active objects
    LList moveableList = level->getActiveMovable();
    LList drawableList = level->getActiveDrawable();
    LList blockList = level->getActiveBlocks();
    
    //instance of LListIterator
    LListIterator iter;
    
    //object that will be checked for its type
    Drawable *object;
    
    iter.init(moveableList);
    
    while ((object = iter.next())) 
    {
        if (((this->right() >= object->left() && this->right() <= object->right()) || (this->left() <= object->right() && this->left() >= object->left())) && (this->bottom() <= object->top()))
        {
            //if Mario runs into an enemy
            if (object->objectType() == goomba_ || object->objectType() == plant_ || object->objectType() == shell_ || object->objectType() == turtle_ || object->objectType() == enemyfireball_)
            {
                //Mario dies from the right
                //go through the changes of Mario's state based on current state
                //and him getting hit by an enemy
                if (isInvincible_)
                {
                    //Mario kills the enemy
                }
                
                else if (this->state_ == FIRE_STATE)
                {
                  this->state_ = BIG_STATE;
                }
                
                else if (this->state_ == BIG_STATE)
                {
                    this->state_ = SMALL_STATE;
                }
                
                else {
                    //Mario dies
                    return false;
                }
                
            }
            //if Mario runs into a Movable Reward
            else if (object->objectType() == mushroom_ || object->objectType() == star_)
            {
                //update Mario's State
                if (object->objectType() == mushroom_) 
                {
                    if (this->state_ == SMALL_STATE)
                    {
                        this->state_ = BIG_STATE;
                    }
                    else
                    {
                        //give Mario Points for getting mushroom
                    }
                    
                }
                else if (object->objectType() == star_)
                {
                    //Mario is invincible 
                    isInvincible_ = true;
                    //Update invincible count
                    starCount_ = 50;
                }
            }
        }
                
        //possibly checking if things fall on Mario's Head
        
         if ((this->top() == object->bottom()) && ((this->right() >= object->left() || this->right() <= object->right()) || (this->left() >= object->left() || this->left() <= object->right())))
         {
             //if an enemy lands on top of Mario
             if (object->objectType() == goomba_ || object->objectType() == shell_ || object->objectType() == turtle_ || object->objectType() == enemyfireball_)
             {
                 if (isInvincible_)
                 {
                     //enemy dies
                 }
                 else 
                 {
                     //Mario dies
                     return false;
                 }
             }
             //if a reward lands on top of Mario
             else if (object->objectType() == mushroom_ || object->objectType() == star_)
             {
                 //update Mario's State
                 if (object->objectType() == mushroom_) 
                 {
                     if (this->state_ == SMALL_STATE)
                     {
                         this->state_ = BIG_STATE;
                     }
                     else
                     {
                         //give Mario Points for getting mushroom
                     }
                     
                 }
                 else if (object->objectType() == star_)
                 {
                     //Mario is invincible 
                     isInvincible_ = true;
                     //Update invicible count
                     starCount_ = 50;
                 }
             }
         }
        
        
        //checks if mario lands on top of object
        if ((this->bottom() == object->top()) && ((this->right() >= object->left() || this->right() <= object->right()) || (this->left() >= object->left() || this->left() <= object->right()))) 
        {
            //if Mario lands on top of an enemy
            if (object->objectType() == goomba_ || object->objectType() == shell_ || object->objectType() == turtle_)
            {
                //enemy dies
                //Mario's jumpcount reset? (Mario will jump higher (bounce))
            }
            //if Mario lands on a reward
            else if (object->objectType() == mushroom_ || object->objectType() == star_)
            {
                //update Mario's State
                if (object->objectType() == mushroom_) 
                {
                    if (this->state_ == SMALL_STATE)
                    {
                        this->state_ = BIG_STATE;
                    }
                    else
                    {
                        //give Mario Points for getting mushroom
                    }
                    
                }
                else if (object->objectType() == star_)
                {
                    //Mario is invincible 
                    isInvincible_ = true;
                    //Update invincible count
                    starCount_ = 50;
                }
            }
            //if Mario lands on an enemy that can't be killed
            else if (object->objectType() == plant_ || object->objectType() == enemyfireball_)
            {
                //go through the changes of Mario's state based on current state
                //and him getting hit by an enemy
                if (this->state_ == FIRE_STATE)
                {
                    this->state_ = BIG_STATE;
                }
                else if (this->state_ == BIG_STATE)
                {
                    this->state_ = SMALL_STATE;
                }
                else {
                    //Mario dies
                    return false;
                }
            }
        }
    }


    iter.init(drawableList);
    
    while ((object = iter.next()))
    {
        if (((this->right() >= object->left() && this->right() <= object->right()) || (this->left() <= object->right() && this->left() >= object->left())) && (this->bottom() <= object->top())) 
        {
            if (object->objectType() == flag_) 
            {
                //end game
            }
            else if (object->objectType() == fireflower_) 
            {
                //update Mario's State
                if (this->state_ == FIRE_STATE)
                {
                    //generate points for fireflower
                }
                else
                {
                    this->state_ = FIRE_STATE;
                }
                    
            }
            else if(object->objectType() == coin_) 
            {
                //update Mario's Points
            }
        }
    }
    
    iter.init(blockList);
    
    while ((object = iter.next()))
    {
        //check if Mario is jumping into a block
        if (this->top() == object->bottom() && ((this->right() >= object->left() || this->right() <= object->right()) || (this->left() >= object->left() || this->left() <= object->right()))) 
        {
            if (this->state_ == BIG_STATE && object->objectType() == breakable_) {
                //break block
            }
            else {
                //stop moving
                if (object->objectType() == question_)
                {
                    //generate reward
                }
            }
        }
        //check if Mario lands on a block
        if (((this->right() >= object->left() && this->right() <= object->right()) || (this->left() <= object->right() && this->left() >= object->left())) && (this->bottom() <= object->top()))
        {
            //stop falling
            //keep moving
        }
    }
    return true;
}
//------------------------------------------------------------
//Creates a fireball
bool Mario::fireball()
{
	return false;
}
