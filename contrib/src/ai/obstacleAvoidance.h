// Filename: obstacleAvoidance.h
// Created by:  Deepak, John, Navin (10Nov2009)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised
// BSD license.  You should have received a copy of this license
// along with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////
#ifndef OBSTACLE_AVOIDANCE_H
#define OBSTACLE_AVOIDANCE_H

#include "aiCharacter.h"
#include "boundingSphere.h"

class AICharacter;

////////////////////////////////////////////////////////////////////
//       Class : ObstacleAvoidance
// Description : This class handles all calls to the obstacle
//               avoidance behavior
////////////////////////////////////////////////////////////////////
class ObstacleAvoidance {
  public :
    AICharacter *_ai_char;
    float _obstacle_avoidance_weight;
    NodePath _nearest_obstacle;
    bool _obstacle_avoidance_done;
    float _feeler;

    ObstacleAvoidance(AICharacter *ai_char, float feeler_length);
    LVecBase3f do_obstacle_avoidance();
    ~ObstacleAvoidance();
    void obstacle_avoidance_activate();
    bool obstacle_detection();
};

#endif