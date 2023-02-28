#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>

class Processor {
 public:
  float Utilization();

 private:
  float prev_working_{0};
  float prev_idle_{0};
};

#endif