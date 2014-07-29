/*
 * libjingle
 * Copyright 2012, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// This file contains structures used for retrieving statistics from an ongoing
// libjingle session.

#ifndef TALK_APP_WEBRTC_STATSTYPES_H_
#define TALK_APP_WEBRTC_STATSTYPES_H_

#include <string>
#include <vector>

#include "webrtc/base/basictypes.h"
#include "webrtc/base/stringencode.h"

namespace webrtc {

class StatsReport {
 public:
  StatsReport() : timestamp(0) { }

  // TODO(tommi): Change this to be an enum type that holds all the
  // kStatsValueName constants.
  typedef const char* StatsValueName;

  std::string id;  // See below for contents.
  std::string type;  // See below for contents.

  struct Value {
    Value() : name(NULL) {}
    // The copy ctor can't be declared as explicit due to problems with STL.
    Value(const Value& other) : name(other.name), value(other.value) {}
    explicit Value(StatsValueName name) : name(name) {}
    Value(StatsValueName name, const std::string& value)
        : name(name), value(value) {
    }

    // TODO(tommi): Remove this operator once we don't need it.
    // The operator is provided for compatibility with STL containers.
    // The public |name| member variable is otherwise meant to be read-only.
    Value& operator=(const Value& other) {
      const_cast<StatsValueName&>(name) = other.name;
      value = other.value;
      return *this;
    }

    // TODO(tommi): Change implementation to do a simple enum value-to-static-
    // string conversion when client code has been updated to use this method
    // instead of the |name| member variable.
    const char* display_name() const { return name; }

    const StatsValueName name;

    std::string value;
  };

  void AddValue(StatsValueName name, const std::string& value);
  void AddValue(StatsValueName name, int64 value);
  template <typename T>
  void AddValue(StatsValueName name, const std::vector<T>& value);
  void AddBoolean(StatsValueName name, bool value);

  void ReplaceValue(StatsValueName name, const std::string& value);

  double timestamp;  // Time since 1970-01-01T00:00:00Z in milliseconds.
  typedef std::vector<Value> Values;
  Values values;

  // TODO(tommi): These should all be enum values.

  // StatsReport types.
  // A StatsReport of |type| = "googSession" contains overall information
  // about the thing libjingle calls a session (which may contain one
  // or more RTP sessions.
  static const char kStatsReportTypeSession[];

  // A StatsReport of |type| = "googTransport" contains information
  // about a libjingle "transport".
  static const char kStatsReportTypeTransport[];

  // A StatsReport of |type| = "googComponent" contains information
  // about a libjingle "channel" (typically, RTP or RTCP for a transport).
  // This is intended to be the same thing as an ICE "Component".
  static const char kStatsReportTypeComponent[];

  // A StatsReport of |type| = "googCandidatePair" contains information
  // about a libjingle "connection" - a single source/destination port pair.
  // This is intended to be the same thing as an ICE "candidate pair".
  static const char kStatsReportTypeCandidatePair[];

  // StatsReport of |type| = "VideoBWE" is statistics for video Bandwidth
  // Estimation, which is global per-session.  The |id| field is "bweforvideo"
  // (will probably change in the future).
  static const char kStatsReportTypeBwe[];

  // StatsReport of |type| = "ssrc" is statistics for a specific rtp stream.
  // The |id| field is the SSRC in decimal form of the rtp stream.
  static const char kStatsReportTypeSsrc[];

  // StatsReport of |type| = "remoteSsrc" is statistics for a specific
  // rtp stream, generated by the remote end of the connection.
  static const char kStatsReportTypeRemoteSsrc[];

  // StatsReport of |type| = "googTrack" is statistics for a specific media
  // track. The |id| field is the track id.
  static const char kStatsReportTypeTrack[];

  // StatsReport of |type| = "iceCandidate" is statistics on a specific
  // ICE Candidate. It links to its transport.
  static const char kStatsReportTypeIceCandidate[];

  // The id of StatsReport of type VideoBWE.
  static const char kStatsReportVideoBweId[];

  // A StatsReport of |type| = "googCertificate" contains an SSL certificate
  // transmitted by one of the endpoints of this connection.  The |id| is
  // controlled by the fingerprint, and is used to identify the certificate in
  // the Channel stats (as "googLocalCertificateId" or
  // "googRemoteCertificateId") and in any child certificates (as
  // "googIssuerId").
  static const char kStatsReportTypeCertificate[];

  // StatsValue names
  static const char kStatsValueNameAudioOutputLevel[];
  static const char kStatsValueNameAudioInputLevel[];
  static const char kStatsValueNameBytesSent[];
  static const char kStatsValueNamePacketsSent[];
  static const char kStatsValueNameBytesReceived[];
  static const char kStatsValueNamePacketsReceived[];
  static const char kStatsValueNamePacketsLost[];
  static const char kStatsValueNameTransportId[];
  static const char kStatsValueNameLocalAddress[];
  static const char kStatsValueNameRemoteAddress[];
  static const char kStatsValueNameWritable[];
  static const char kStatsValueNameReadable[];
  static const char kStatsValueNameActiveConnection[];


  // Internal StatsValue names
  static const char kStatsValueNameAvgEncodeMs[];
  static const char kStatsValueNameEncodeRelStdDev[];
  static const char kStatsValueNameEncodeUsagePercent[];
  static const char kStatsValueNameCaptureJitterMs[];
  static const char kStatsValueNameCaptureQueueDelayMsPerS[];
  static const char kStatsValueNameCodecName[];
  static const char kStatsValueNameBandwidthLimitedResolution[];
  static const char kStatsValueNameCpuLimitedResolution[];
  static const char kStatsValueNameViewLimitedResolution[];
  static const char kStatsValueNameAdaptationChanges[];
  static const char kStatsValueNameEchoCancellationQualityMin[];
  static const char kStatsValueNameEchoDelayMedian[];
  static const char kStatsValueNameEchoDelayStdDev[];
  static const char kStatsValueNameEchoReturnLoss[];
  static const char kStatsValueNameEchoReturnLossEnhancement[];
  static const char kStatsValueNameExpandRate[];
  static const char kStatsValueNameFirsReceived[];
  static const char kStatsValueNameFirsSent[];
  static const char kStatsValueNameFrameHeightInput[];
  static const char kStatsValueNameFrameHeightReceived[];
  static const char kStatsValueNameFrameHeightSent[];
  static const char kStatsValueNameFrameRateReceived[];
  static const char kStatsValueNameFrameRateDecoded[];
  static const char kStatsValueNameFrameRateOutput[];
  static const char kStatsValueNameDecodeMs[];
  static const char kStatsValueNameMaxDecodeMs[];
  static const char kStatsValueNameCurrentDelayMs[];
  static const char kStatsValueNameTargetDelayMs[];
  static const char kStatsValueNameJitterBufferMs[];
  static const char kStatsValueNameMinPlayoutDelayMs[];
  static const char kStatsValueNameRenderDelayMs[];
  static const char kStatsValueNameCaptureStartNtpTimeMs[];
  static const char kStatsValueNameFrameRateInput[];
  static const char kStatsValueNameFrameRateSent[];
  static const char kStatsValueNameFrameWidthInput[];
  static const char kStatsValueNameFrameWidthReceived[];
  static const char kStatsValueNameFrameWidthSent[];
  static const char kStatsValueNameJitterReceived[];
  static const char kStatsValueNameNacksReceived[];
  static const char kStatsValueNameNacksSent[];
  static const char kStatsValueNamePlisReceived[];
  static const char kStatsValueNamePlisSent[];
  static const char kStatsValueNamePreferredJitterBufferMs[];
  static const char kStatsValueNameRtt[];
  static const char kStatsValueNameAvailableSendBandwidth[];
  static const char kStatsValueNameAvailableReceiveBandwidth[];
  static const char kStatsValueNameTargetEncBitrate[];
  static const char kStatsValueNameActualEncBitrate[];
  static const char kStatsValueNameRetransmitBitrate[];
  static const char kStatsValueNameTransmitBitrate[];
  static const char kStatsValueNameBucketDelay[];
  static const char kStatsValueNameInitiator[];
  static const char kStatsValueNameTransportType[];
  static const char kStatsValueNameContentName[];
  static const char kStatsValueNameComponent[];
  static const char kStatsValueNameChannelId[];
  static const char kStatsValueNameTrackId[];
  static const char kStatsValueNameSsrc[];
  static const char kStatsValueNameTypingNoiseState[];
  static const char kStatsValueNameDer[];
  static const char kStatsValueNameFingerprint[];
  static const char kStatsValueNameFingerprintAlgorithm[];
  static const char kStatsValueNameIssuerId[];
  static const char kStatsValueNameLocalCertificateId[];
  static const char kStatsValueNameRemoteCertificateId[];
  static const char kStatsValueNameLocalCandidateType[];
  static const char kStatsValueNameRemoteCandidateType[];
  static const char kStatsValueNameRecvPacketGroupArrivalTimeDebug[];
  static const char kStatsValueNameRecvPacketGroupPropagationDeltaDebug[];
  static const char kStatsValueNameRecvPacketGroupPropagationDeltaSumDebug[];
  static const char kStatsValueNameDecodingCTSG[];
  static const char kStatsValueNameDecodingCTN[];
  static const char kStatsValueNameDecodingNormal[];
  static const char kStatsValueNameDecodingPLC[];
  static const char kStatsValueNameDecodingCNG[];
  static const char kStatsValueNameDecodingPLCCNG[];
};

typedef std::vector<StatsReport> StatsReports;

}  // namespace webrtc

#endif  // TALK_APP_WEBRTC_STATSTYPES_H_
