/* Generated by Together */

#include "OrderedTaskPoint.hpp"
#include "TaskLeg.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>


// -------

GEOPOINT OrderedTaskPoint::get_reference_travelled()
{
  if (has_entered()) {
    return getMaxLocation();
  } else {
    return getLocation();
  }
}

GEOPOINT OrderedTaskPoint::get_reference_scored()
{
  return getLocation();
}

GEOPOINT OrderedTaskPoint::get_reference_nominal()
{
  return getLocation();
}

GEOPOINT OrderedTaskPoint::get_reference_remaining()
{
  if (has_entered()) {
    return getMinLocation();
  } else {
    return getLocation();
  }
}

// -------


TaskLeg* OrderedTaskPoint::get_leg_in() {
  return leg_in;
}

TaskLeg* OrderedTaskPoint::get_leg_out() {
  return leg_out;
}

void OrderedTaskPoint::set_leg_in(TaskLeg* the_leg)
{
  leg_in = the_leg;
}

void OrderedTaskPoint::set_leg_out(TaskLeg* the_leg) {
  leg_out = the_leg;
}

bool OrderedTaskPoint::scan_active(OrderedTaskPoint* atp) {
  // reset
  active_state = NOTFOUND_ACTIVE;

  if (atp == this) {
    active_state = CURRENT_ACTIVE;
  } else if (leg_in 
             && ((leg_in->get_origin()->getActiveState() 
                  == CURRENT_ACTIVE) 
                 || (leg_in->get_origin()->getActiveState() 
                     == AFTER_ACTIVE))) {
    active_state = AFTER_ACTIVE;
  } else {
    active_state = BEFORE_ACTIVE;
  }

  if (leg_out) { 
    // propagate to remainder of task
    return leg_out->get_destination()->scan_active(atp);
  } else if (active_state == BEFORE_ACTIVE) {
    return false;
  } else {
    return true;
  }
}

//////////


void OrderedTaskPoint::scan_bearing_remaining(const GEOPOINT &ref) 
{
  if (leg_out) {
    leg_out->get_destination()->scan_bearing_remaining(ref);
  } 
  if (leg_in) {
    bearing_remaining = leg_in->leg_bearing_remaining(ref);
  } else {
    bearing_remaining = ::Bearing(ref, getLocation()); // bearing to start
  }
}

void OrderedTaskPoint::scan_bearing_travelled(const GEOPOINT &ref) 
{
  if (leg_out) {
    leg_out->get_destination()->scan_bearing_travelled(ref);
  } 
  if (leg_in) {
    bearing_travelled = leg_in->leg_bearing_travelled(ref);
  } else if (leg_out) {
    bearing_travelled = leg_out->leg_bearing_travelled(ref);
  } else {
    bearing_travelled = 0.0;
  }
}


//////////////////

double OrderedTaskPoint::scan_distance_remaining(const GEOPOINT &ref) 
{
  // distance remaining from the given task point
  // (accumulates towards start)

  if (leg_in) {
    this_distance_remaining = leg_in->leg_distance_remaining(ref);
  } else {
    this_distance_remaining = 0.0;
  }

  if (leg_out) {
    double d = leg_out->leg_distance_remaining(ref);
    distance_remaining = 
      leg_out->get_destination()->scan_distance_remaining(ref)
      +d;
  } else {
    // finish, reset
    distance_remaining = 0.0;
  }
  return distance_remaining;
}


////

double OrderedTaskPoint::scan_distance_nominal() 
{
  // distance from start to the task point
  // (accumulates towards finish)

  if (!leg_in) {
    // start, reset
    distance_nominal = 0.0;
  }
  if (leg_out) {
    double d = leg_out->leg_distance_nominal();
    leg_out->get_destination()->distance_nominal = d+distance_nominal;
    return leg_out->get_destination()->scan_distance_nominal();
  } else {
    // return at end
    return distance_nominal;
  }
}

double OrderedTaskPoint::scan_distance_planned() 
{
  // distance from start to the task point
  // (accumulates towards finish)
  if (leg_in) {
    this_distance_planned = leg_in->leg_distance_planned();
    bearing_planned = leg_in->leg_bearing_planned();
  } else {
    this_distance_planned = 0.0;
    bearing_planned = 0.0;
  }

  if (leg_in) {
    distance_planned = leg_in->leg_distance_planned()
      +leg_in->get_origin()->distance_planned;
  } else {
    distance_planned = 0;
  }
  if (leg_out) {
    return leg_out->get_destination()->scan_distance_planned();
  } else {
    return distance_planned;
  }
}


double OrderedTaskPoint::scan_distance_travelled(const GEOPOINT &ref) 
{
  if (leg_in) {
    this_distance_travelled = leg_in->leg_distance_travelled(ref);
  } else {
    this_distance_travelled = 0.0;
  }

  if (leg_in) {
    distance_travelled = leg_in->leg_distance_travelled(ref)
      +leg_in->get_origin()->distance_travelled;
  } else {
    distance_travelled = 0;
  }
  if (leg_out) {
    return leg_out->get_destination()->scan_distance_travelled(ref);
  } else {
    return distance_travelled;
  }
}


double OrderedTaskPoint::scan_distance_scored(const GEOPOINT &ref) 
{
  if (leg_in) {
    distance_scored = leg_in->leg_distance_scored(ref)
      +leg_in->get_origin()->distance_scored;
  } else {
    distance_scored = 0;
  }
  if (leg_out) {
    return leg_out->get_destination()->scan_distance_scored(ref);
  } else {
    return distance_scored;
  }
}


bool 
OrderedTaskPoint::transition_enter(const AIRCRAFT_STATE & ref_now, 
                                   const AIRCRAFT_STATE & ref_last)
{
  bool entered = ObservationZone::transition_enter(ref_now, ref_last);
  if (entered) {
    state_entered = ref_now;
  }
  return entered;
}

bool 
OrderedTaskPoint::transition_exit(const AIRCRAFT_STATE & ref_now, 
                                  const AIRCRAFT_STATE & ref_last)
{
  bool exited = ObservationZone::transition_exit(ref_now, ref_last);
  if (exited) {
    state_exited = ref_last;
  }
  return exited;
}


void 
OrderedTaskPoint::print(std::ostream& f) const
{
  SampledTaskPoint::print(f);
  f << "# Bearing travelled " << bearing_travelled << "\n";
  f << "# Distance travelled " << this_distance_travelled << "\n";
  f << "# Bearing remaining " << bearing_remaining << "\n";
  f << "# Distance remaining " << this_distance_remaining << "\n";
  f << "# Bearing planned " << bearing_planned << "\n";
  f << "# Distance planned " << this_distance_planned << "\n";
  f << "# Entered " << state_entered.Time << "\n";
}

const std::vector<SearchPoint>& 
OrderedTaskPoint::get_search_points()
{
  if (active_state== BEFORE_ACTIVE) {
    return SampledTaskPoint::get_search_points(true);
  } else if (active_state == CURRENT_ACTIVE) {
    return SampledTaskPoint::get_search_points(false);
  } else {
    return get_boundary_points();
  }
}


double OrderedTaskPoint::get_distance_remaining(const AIRCRAFT_STATE &)
{
  return this_distance_remaining;
}

double 
OrderedTaskPoint::get_bearing_remaining(const AIRCRAFT_STATE &)
{
  return bearing_remaining;
}


GLIDE_RESULT OrderedTaskPoint::glide_solution_travelled(const AIRCRAFT_STATE &ac, 
                                                        const MacCready &msolv,
                                                        const double minH)
{
  GLIDE_STATE gs;
  gs.Distance = get_distance_travelled();
  gs.Bearing = get_bearing_travelled();
  gs.MinHeight = std::max(minH,getElevation());

  return msolv.solve(ac,gs);
}

GLIDE_RESULT OrderedTaskPoint::glide_solution_planned(const AIRCRAFT_STATE &ac, 
                                                      const MacCready &msolv,
                                                      const double minH)
{
  GLIDE_STATE gs;
  gs.Distance = get_distance_planned();
  gs.Bearing = get_bearing_planned();
  gs.MinHeight = std::max(minH,getElevation());

  return msolv.solve(ac,gs);
}


double OrderedTaskPoint::double_leg_distance(const GEOPOINT &ref) const
{
  assert(leg_in);
  assert(leg_out);
  return 
    ::Distance(leg_in->get_origin()->get_reference_remaining(), 
               ref)+
    ::Distance(ref,
               leg_out->get_destination()->get_reference_remaining());
}
