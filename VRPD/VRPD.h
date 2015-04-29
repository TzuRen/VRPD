//
//  VRPD.h
//  VRPD
//
//  Created by Xiaoya Wei on 4/26/15.
//  Copyright (c) 2015 Xiaoya Wei. All rights reserved.
//

#ifndef __VRPD__VRPD__
#define __VRPD__VRPD__

#include <stdio.h>
#include "fstream"
#include "Coordinate.h"
#include "iostream"
#include "cmath"
#include "vector"
#include "Solver.h"

class VRPD{
public:
    VRPD(const Solver& solver);
    bool readFile(const char *filename);
    double deployAllDrones(const int *nextArray, const int *preArray);
    int getNumByTruck();
    void reset();
    void getDroneAssignment(bool *drone);
    int globalIndexToLocal(int index);
    int localIndexToGlobal(int index);
    ~VRPD();
    
private:
    DroneDeployment getDroneDeployment(const int dest, const int *route, const int *deployedDrones, const double *waitingTime, const int length, const int routeID) const;
    void sendDrone(int *route, int *deployedDrones, double *waitingTime, const DroneDeployment& deply);
    void withdrawDrone(int *route, int *deployedDrones, double *waitingTime, const DroneDeployment& deply);
//    bool closerThan(const double x1, const double x2) const;
    int newIndex(int index) const;
    int originalIndex(int index) const;
    void createRouteInfo();
    double getBasicRouteTime(const int *route, const int len) const;
    void assignInitialDrones(int method = 1);
    
    
    int numFleet, numDrone, depotIndex, truckCap, numCustomer, numOfRoute = 0, numByTruck, numByDrone;
    double tDrone, alpha;
    double **dist;
    double **cost;
    Coordinate* nodes;
    bool *droned;
    int *lengthOfRoute, **deployedDrone, *nextArray, *preArray, **truckRoutes, indexOffset;
    const double INF = 1e30;
    const double beta = 0; //balance segment length and drone flying duration
    DroneDeployment *droneDplyAtNode;
    std::vector<DroneDeployment> *droneDplyAtRoute;
    double *timeOfRoute, **waitingTime;
};

#endif /* defined(__VRPD__VRPD__) */
