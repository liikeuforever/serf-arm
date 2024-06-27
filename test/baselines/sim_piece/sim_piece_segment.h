#ifndef SIM_PIECE_SIM_PIECE_SEGMENT_H_
#define SIM_PIECE_SIM_PIECE_SEGMENT_H_

class SimPieceSegment {
 public:
  SimPieceSegment(long init_timestamp, double aMin, double aMax, double b): kInitTimestamp(init_timestamp), kAMin
      (aMin), kAMax(aMax), kA((aMin + aMax) / 2), kB(b) {}

  SimPieceSegment(long init_timestamp, double a, double b): SimPieceSegment(init_timestamp, a, a, b) {}

  long GetInitTimestamp() {
    return kInitTimestamp;
  }

  double GetAMin() {
    return kAMin;
  }

  double GetAMax() {
    return kAMax;
  }

  double GetA() {
    return kA;
  }

  double GetB() {
    return kB;
  }

 private:
  const long kInitTimestamp;
  const double kAMin;
  const double kAMax;
  const double kA;
  const double kB;
};

#endif // SIM_PIECE_SIM_PIECE_SEGMENT_H_
