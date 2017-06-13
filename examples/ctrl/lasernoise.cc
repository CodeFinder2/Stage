/*
   File rangernoise.cc: (configurable) ranger noise plugin demo for Stage
   Author: Richard Vaughan, Adrian Böckenkamp
   Date: 13 Jun 2017
*/

#include "stage.hh"

#include <string>
#include <vector>
#include <random>

using namespace std;
using namespace Stg;

double mean = 0.0;
double stddev = 0.03;

// process the ranger data
int RangerUpdate(ModelRanger *mod, void *)
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::normal_distribution<double> guassian_noise(mean, stddev);
  // See http://en.cppreference.com/w/cpp/numeric/random/normal_distribution

  // make the data noisy:
  ModelRanger::Sensor &sensor = mod->GetSensorsMutable()[0];
  for (auto &it : sensor.ranges) {
    if (it < sensor.range.max) { // don't make the range limit noisy (like in most real life cases)
      it += guassian_noise(gen);
    }
  }

  return 0; // run again
}

void SplitArgs(const string &text, const string &separators, vector<string> &words)
{
  int n = text.length();
  int start = text.find_first_not_of(separators);
  while ((start >= 0) && (start < n)) {
    int stop = text.find_first_of(separators, start);
    if ((stop < 0) || (stop > n)) {
      stop = n;
    }
    words.push_back(text.substr(start, stop - start));
    start = text.find_first_not_of(separators, stop + 1);
  }
}

// Stage calls this when the model starts up. we just add a callback to
// the model that gets called just after the sensor update is done.
extern "C" int Init(Model *mod, CtrlArgs *args)
{
  // tokenize the argument string into words
  vector<string> words;
  SplitArgs(args->worldfile, string(" \t"), words); // words[0] is the plugin name (= "lasernoise")
  if (words.size() == 2) { // just one parameter? consider this to be standard deviation
    stddev = atof(words[1].c_str());
    // PRINT_MSG1("Setting stddev=%f of LaserNoiseV2 plugin.", stddev);
  } else if (words.size() >= 3) { // two parameters given? this is mean and std. dev.
    mean = atof(words[1].c_str());
    stddev = atof(words[2].c_str());
    // PRINT_MSG3("Setting mean=%f, stddev=%f of '%s' plugin.", mean, stddev, words[0].c_str());
  }

  mod->AddCallback(Model::CB_UPDATE, model_callback_t(RangerUpdate), NULL);

  // add this so we can see the effects immediately, without needing
  // anyone else to subscribe to the ranger
  mod->Subscribe();

  return 0; // ok
}
