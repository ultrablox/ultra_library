
#ifndef ULTRA_LIB_PROGRESS_INDICATOR_H
#define ULTRA_LIB_PROGRESS_INDICATOR_H

#include <chrono>
#include <iostream>

class progress_indicator
{
  using tp_t = std::chrono::system_clock::time_point;
public:
  progress_indicator(unsigned max_val, unsigned secs_between_prints = 5)
	  :start(std::chrono::system_clock::now()), maxVal(max_val), maxPercent(0), secsBetweenPrints(secs_between_prints), lastPrintTP(start)
  {
  }

  void submit_progress(unsigned cur_val)
  {
    tp_t cur_tp = std::chrono::system_clock::now();
    float cur_percent = 100.0f * cur_val / maxVal;

	if (((unsigned)cur_percent > maxPercent) || (std::chrono::duration_cast<std::chrono::seconds>(cur_tp - lastPrintTP).count() > secsBetweenPrints))
    {
      maxPercent = (unsigned)cur_percent;

      unsigned secs = std::chrono::duration_cast<std::chrono::seconds>(cur_tp - start).count();

      float estimated = secs / cur_percent * 100.f - secs;
      std::cout << "Progress " << cur_percent << "%, " << secs << " seconds passed, " << estimated << " left" << std::endl;
	  lastPrintTP = cur_tp;
    }
  }
private:
  tp_t start, lastPrintTP;
  unsigned maxVal;
  unsigned maxPercent, secsBetweenPrints;
};

#endif
