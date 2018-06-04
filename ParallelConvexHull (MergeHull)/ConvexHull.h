#pragma once
#include <math.h>
#include <vector>
#include <algorithm>
#include <utility>
#include <thread>
#include <iostream>
#include <random>
#include <string>
#include "Geometry.h"
#include<stack>

using namespace std;

typedef  vector<Point> Hull;

const size_t CORES = 4;

void grahamScan(vector<Point>& points, Hull & result_hull) 
{
	int size = points.size();
	stack<Point> hull;
	//hull of three points
	if (size < 3)
		return;

	int leastX = 0;
	for (int i = 1; i < size; i++)
		if (points[i] < points[leastX])
			leastX = i;

	// swap the pivot with the first point
	iter_swap(points.begin(), points.begin() + leastX);

	// sort the remaining point according to polar order about the pivot
	Point pivot = points[0];
	sort(points.begin()+1, points.end(), [pivot](const Point & a, const Point & b) -> bool
	{
		float turn = Point::leftTurn(pivot,a, b);
		if (turn > 0) return false;
		if (turn < 0) return true;
		return  Point::sqrdistance(pivot, a) < Point::sqrdistance(pivot, b);
	});

	hull.push(points[0]);
	hull.push(points[1]);
	hull.push(points[2]);

	for (int i = 3; i < size; i++) {
		if (hull.size() >= 2)
		{
			Point prev_top = hull.top();
			hull.pop();
			//remove last convex point till we find left turn
			while (hull.size()!=0 && Point::leftTurn(hull.top(), prev_top, points[i]) >= 0) {
				prev_top = hull.top();
				hull.pop();
			}
			hull.push(prev_top);
		}
		hull.push(points[i]);
	}

	Hull v;
	while (!hull.empty())
	{
		v.push_back(hull.top());
		hull.pop();
	}
	reverse(v.begin(), v.end());
	//repeat last point
	v.push_back(v[0]);
	
	result_hull=v;
}

void mergeHulls(const Hull & left, const Hull & right , Hull & result_hull)
{
	//find right point of left hull
	size_t maxXIndex = 0;
	float maxX = left[0].x;
	for (int i = 0; i < left.size(); ++i)
	{
		if (maxX < left[i].x)
		{
			maxXIndex = i;
			maxX = left[i].x;
		}
	}

	//indexes : left-upper, right-upper, left-lower and right-lower
	size_t lu = maxXIndex, ru = 0, ll = maxXIndex, rl = right.size() - 1;
	
	while (	( lu != 0 && Point::leftTurn( right[ru], left[lu], left[lu - 1]) <= 0 )
			|| ( ru != right.size( ) - 1 && Point::leftTurn( left[lu], right[ru] ,right[ ru + 1 ] ) >= 0 ) )
	{
		if (lu != 0 && Point::leftTurn(right[ru], left[lu], left[lu - 1])<=0)
			--lu;
		else
			++ru;
	}
	while (	(ll != left.size() - 1 && Point::leftTurn( right[rl], left[ll], left[ll + 1]) >= 0)
			|| ( rl != 0  && Point::leftTurn(left[ll], right[rl], right[rl - 1]) <= 0) )
	{
		if (ll != left.size() - 1 && Point::leftTurn(right[rl], left[ll], left[ll + 1]) >= 0)
			++ll;
		else
			--rl;
	}
	
	Hull result(lu+1+rl-ru+1+left.size()-ll);
	copy(left.begin(), left.begin() + lu + 1, result.begin());
	copy(right.begin()+ru, right.begin() + rl + 1, result.begin()+ lu + 1);
	copy(left.begin()+ll, left.end(), result.begin() + (lu + 1 + rl - ru + 1) );

	result_hull=result;

}

void parallelMergeHull(vector<Point>& points,Hull & result_hull)
{
	if (points.size() < 500)
	{
		grahamScan(points, result_hull);
		return;
	}

	vector<vector<Point>> in_points(CORES);
	vector<Hull> hulls(CORES);
	size_t size = points.size();
	
	std::thread * threads = new thread[CORES];

	for (int i = 0; i < CORES; ++i) 
	{
		in_points[i] = vector<Point>(points.begin() + (size*i / CORES), points.begin() + (size*(i + 1) / CORES));
		
	}
	for (int i = 0; i < CORES; ++i)
	{
		threads[i] = std::thread(parallelMergeHull, std::ref(in_points[i]), std::ref(hulls[i]));
		//parallelMergeHull(in_points[i], hulls[i]);
	}
	for (int i = 0; i < CORES; ++i) {
		threads[i].join();
	}
	//convex hulls calculated , now we need to merge them
	while (hulls.size()>1)
	{
		vector<Hull> new_hulls;
		for (int i = 0; i+1 < hulls.size(); i = i + 2)
		{
			Hull merged_hull;
			mergeHulls(hulls[i], hulls[i + 1], merged_hull);
			new_hulls.push_back(merged_hull);
		}
		if (hulls.size() % 2 == 1)
		{
			new_hulls.push_back(hulls[hulls.size()-1]);
		}
		hulls = new_hulls;
	}
	result_hull = hulls[0];
}

std::vector<Point> create_test_vector(const size_t size)
{
	std::vector<Point> result_vec(size);
	for (int i = 0; i < size; ++i)
	{
		float x = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f) * 10000.0f;
		float y = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f) * 10000.0f;
		result_vec[i] = { x,y };
	}
	std::sort(result_vec.begin(), result_vec.end());
	return result_vec;
}
