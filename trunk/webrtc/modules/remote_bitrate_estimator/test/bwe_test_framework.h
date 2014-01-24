/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_REMOTE_BITRATE_ESTIMATOR_TEST_BWE_TEST_FRAMEWORK_H_
#define WEBRTC_MODULES_REMOTE_BITRATE_ESTIMATOR_TEST_BWE_TEST_FRAMEWORK_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <list>
#include <numeric>
#include <string>
#include <vector>

#include "webrtc/modules/interface/module_common_types.h"
#include "webrtc/modules/remote_bitrate_estimator/test/bwe_test_logging.h"

namespace webrtc {
namespace testing {
namespace bwe {

template<typename T> class Stats {
 public:
  Stats()
      : data_(),
        last_mean_count_(0),
        last_variance_count_(0),
        last_minmax_count_(0),
        mean_(0),
        variance_(0),
        min_(0),
        max_(0) {
  }

  void Push(T data_point) {
    data_.push_back(data_point);
  }

  T GetMean() {
    if (last_mean_count_ != data_.size()) {
      last_mean_count_ = data_.size();
      mean_ = std::accumulate(data_.begin(), data_.end(), static_cast<T>(0));
      assert(last_mean_count_ != 0);
      mean_ /= static_cast<T>(last_mean_count_);
    }
    return mean_;
  }
  T GetVariance() {
    if (last_variance_count_ != data_.size()) {
      last_variance_count_ = data_.size();
      T mean = GetMean();
      variance_ = 0;
      for (typename std::vector<T>::const_iterator it = data_.begin();
          it != data_.end(); ++it) {
        T diff = (*it - mean);
        variance_ += diff * diff;
      }
      assert(last_variance_count_ != 0);
      variance_ /= static_cast<T>(last_variance_count_);
    }
    return variance_;
  }
  T GetStdDev() {
    return std::sqrt(static_cast<double>(GetVariance()));
  }
  T GetMin() {
    RefreshMinMax();
    return min_;
  }
  T GetMax() {
    RefreshMinMax();
    return max_;
  }

  void Log(const std::string& units) {
    BWE_TEST_LOGGING_LOG5("", "%f %s\t+/-%f\t[%f,%f]",
        GetMean(), units.c_str(), GetStdDev(), GetMin(), GetMax());
  }

 private:
  void RefreshMinMax() {
    if (last_minmax_count_ != data_.size()) {
      last_minmax_count_ = data_.size();
      min_ = max_ = 0;
      if (data_.empty()) {
        return;
      }
      typename std::vector<T>::const_iterator it = data_.begin();
      min_ = max_ = *it;
      while (++it != data_.end()) {
        min_ = std::min(min_, *it);
        max_ = std::max(max_, *it);
      }
    }
  }

  std::vector<T> data_;
  typename std::vector<T>::size_type last_mean_count_;
  typename std::vector<T>::size_type last_variance_count_;
  typename std::vector<T>::size_type last_minmax_count_;
  T mean_;
  T variance_;
  T min_;
  T max_;
};

class Random {
 public:
  explicit Random(uint32_t seed);

  // Return pseudo random number in the interval [0.0, 1.0].
  float Rand();

  // Normal Distribution.
  int Gaussian(int mean, int standard_deviation);

  // TODO(solenberg): Random from histogram.
  // template<typename T> int Distribution(const std::vector<T> histogram) {

 private:
  uint32_t a_;
  uint32_t b_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(Random);
};

class Packet {
 public:
  Packet();
  Packet(int64_t send_time_us, uint32_t payload_size,
            const RTPHeader& header);
  Packet(int64_t send_time_us, uint32_t sequence_number);

  bool operator<(const Packet& rhs) const;

  void set_send_time_us(int64_t send_time_us);
  int64_t send_time_us() const { return send_time_us_; }
  uint32_t payload_size() const { return payload_size_; }
  const RTPHeader& header() const { return header_; }

 private:
  int64_t send_time_us_;   // Time the packet left last processor touching it.
  uint32_t payload_size_;  // Size of the (non-existent, simulated) payload.
  RTPHeader header_;       // Actual contents.
};

typedef std::list<Packet> Packets;
typedef std::list<Packet>::iterator PacketsIt;
typedef std::list<Packet>::const_iterator PacketsConstIt;

bool IsTimeSorted(const Packets& packets);

class PacketProcessor;

class PacketProcessorListener {
 public:
  virtual ~PacketProcessorListener() {}

  virtual void AddPacketProcessor(PacketProcessor* processor) = 0;
  virtual void RemovePacketProcessor(PacketProcessor* processor) = 0;
};

class PacketProcessor {
 public:
  explicit PacketProcessor(PacketProcessorListener* listener);
  virtual ~PacketProcessor();

  // Run simulation for |time_ms| micro seconds, consuming packets from, and
  // producing packets into in_out. The outgoing packet list must be sorted on
  // |send_time_us_|. The simulation time |time_ms| is optional to use.
  virtual void RunFor(int64_t time_ms, Packets* in_out) = 0;

 private:
  PacketProcessorListener* listener_;

  DISALLOW_COPY_AND_ASSIGN(PacketProcessor);
};

class RateCounterFilter : public PacketProcessor {
 public:
  explicit RateCounterFilter(PacketProcessorListener* listener);
  virtual ~RateCounterFilter();

  uint32_t packets_per_second() const { return packets_per_second_; }
  uint32_t bits_per_second() const { return bytes_per_second_ * 8; }

  void LogStats();
  virtual void RunFor(int64_t time_ms, Packets* in_out);

 private:
  const int64_t kWindowSizeUs;
  uint32_t packets_per_second_;
  uint32_t bytes_per_second_;
  int64_t last_accumulated_us_;
  Packets window_;
  Stats<double> pps_stats_;
  Stats<double> kbps_stats_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(RateCounterFilter);
};

class LossFilter : public PacketProcessor {
 public:
  explicit LossFilter(PacketProcessorListener* listener);
  virtual ~LossFilter() {}

  void SetLoss(float loss_percent);
  virtual void RunFor(int64_t time_ms, Packets* in_out);

 private:
  Random random_;
  float loss_fraction_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(LossFilter);
};

class DelayFilter : public PacketProcessor {
 public:
  explicit DelayFilter(PacketProcessorListener* listener);
  virtual ~DelayFilter() {}

  void SetDelay(int64_t delay_ms);
  virtual void RunFor(int64_t time_ms, Packets* in_out);

 private:
  int64_t delay_us_;
  int64_t last_send_time_us_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(DelayFilter);
};

class JitterFilter : public PacketProcessor {
 public:
  explicit JitterFilter(PacketProcessorListener* listener);
  virtual ~JitterFilter() {}

  void SetJitter(int64_t stddev_jitter_ms);
  virtual void RunFor(int64_t time_ms, Packets* in_out);

 private:
  Random random_;
  int64_t stddev_jitter_us_;
  int64_t last_send_time_us_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(JitterFilter);
};

class ReorderFilter : public PacketProcessor {
 public:
  explicit ReorderFilter(PacketProcessorListener* listener);
  virtual ~ReorderFilter() {}

  void SetReorder(float reorder_percent);
  virtual void RunFor(int64_t time_ms, Packets* in_out);

 private:
  Random random_;
  float reorder_fraction_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(ReorderFilter);
};

// Apply a bitrate choke with an infinite queue on the packet stream.
class ChokeFilter : public PacketProcessor {
 public:
  explicit ChokeFilter(PacketProcessorListener* listener);
  virtual ~ChokeFilter() {}

  void SetCapacity(uint32_t kbps);
  void SetMaxDelay(int64_t max_delay_ms);
  virtual void RunFor(int64_t time_ms, Packets* in_out);

 private:
  uint32_t kbps_;
  int64_t max_delay_us_;
  int64_t last_send_time_us_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(ChokeFilter);
};

class TraceBasedDeliveryFilter : public PacketProcessor {
 public:
  explicit TraceBasedDeliveryFilter(PacketProcessorListener* listener);
  virtual ~TraceBasedDeliveryFilter() {}

  // The file should contain nanosecond timestamps corresponding to the time
  // when the network can accept another packet. The timestamps should be
  // separated by new lines, e.g., "100000000\n125000000\n321000000\n..."
  bool Init(const std::string& filename);
  virtual void RunFor(int64_t time_ms, Packets* in_out);

 private:
  void ProceedToNextSlot();

  typedef std::vector<int64_t> TimeList;
  TimeList delivery_times_us_;
  TimeList::const_iterator next_delivery_it_;
  int64_t local_time_us_;

  DISALLOW_COPY_AND_ASSIGN(TraceBasedDeliveryFilter);
};

class PacketSender : public PacketProcessor {
 public:
  struct Feedback {
    uint32_t estimated_bps;
  };

  explicit PacketSender(PacketProcessorListener* listener);
  virtual ~PacketSender() {}

  virtual uint32_t GetCapacityKbps() const { return 0; }

  // Call GiveFeedback() with the returned interval in milliseconds, provided
  // there is a new estimate available.
  virtual int64_t GetFeedbackIntervalMs() const { return 1000; }
  virtual void GiveFeedback(const Feedback& feedback) {}

 private:
  DISALLOW_COPY_AND_ASSIGN(PacketSender);
};

struct PacketSenderFactory {
  PacketSenderFactory() {}
  virtual ~PacketSenderFactory() {}
  virtual PacketSender* Create() const = 0;
};

class VideoSender : public PacketSender {
 public:
  VideoSender(PacketProcessorListener* listener, float fps, uint32_t kbps,
              uint32_t ssrc, float first_frame_offset);
  virtual ~VideoSender() {}

  uint32_t max_payload_size_bytes() const { return kMaxPayloadSizeBytes; }
  uint32_t bytes_per_second() const { return bytes_per_second_; }

  virtual uint32_t GetCapacityKbps() const;

  // TODO(solenberg): void SetFrameRate(float fps);
  // TODO(solenberg): void SetRate(uint32_t kbps);
  virtual void RunFor(int64_t time_ms, Packets* in_out);

 private:
  const uint32_t kMaxPayloadSizeBytes;
  const uint32_t kTimestampBase;
  double frame_period_ms_;
  double next_frame_ms_;
  double now_ms_;
  uint32_t bytes_per_second_;
  uint32_t frame_size_bytes_;
  RTPHeader prototype_header_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(VideoSender);
};
}  // namespace bwe
}  // namespace testing
}  // namespace webrtc

#endif  // WEBRTC_MODULES_REMOTE_BITRATE_ESTIMATOR_TEST_BWE_TEST_FRAMEWORK_H_
