// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/quic/quic_stream_factory.h"

#include "base/run_loop.h"
#include "base/string_util.h"
#include "net/dns/mock_host_resolver.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_response_info.h"
#include "net/http/http_util.h"
#include "net/quic/crypto/quic_decrypter.h"
#include "net/quic/crypto/quic_encrypter.h"
#include "net/quic/quic_http_stream.h"
#include "net/quic/test_tools/mock_clock.h"
#include "net/quic/test_tools/mock_crypto_client_stream_factory.h"
#include "net/quic/test_tools/mock_random.h"
#include "net/quic/test_tools/quic_test_utils.h"
#include "net/socket/socket_test_util.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace net {
namespace test {

class QuicStreamFactoryTest : public ::testing::Test {
 protected:
  QuicStreamFactoryTest()
      : clock_(new MockClock()),
        factory_(&host_resolver_, &socket_factory_,
                 &crypto_client_stream_factory_,
                 &random_generator_, clock_),
        host_port_proxy_pair_(HostPortPair("www.google.com", 443),
                              ProxyServer::Direct()) {
  }

  scoped_ptr<QuicEncryptedPacket> ConstructRstPacket(
      QuicPacketSequenceNumber num,
      QuicStreamId stream_id) {
    QuicPacketHeader header;
    header.public_header.guid = 0xDEADBEEF;
    header.public_header.reset_flag = false;
    header.public_header.version_flag = true;
    header.packet_sequence_number = num;
    header.entropy_flag = false;
    header.fec_flag = false;
    header.fec_group = 0;

    QuicRstStreamFrame rst(stream_id, QUIC_STREAM_NO_ERROR);
    return scoped_ptr<QuicEncryptedPacket>(
        ConstructPacket(header, QuicFrame(&rst)));
  }

  scoped_ptr<QuicEncryptedPacket> ConstructAckPacket(
      QuicPacketSequenceNumber largest_received,
      QuicPacketSequenceNumber least_unacked) {
    QuicPacketHeader header;
    header.public_header.guid = 0xDEADBEEF;
    header.public_header.reset_flag = false;
    header.public_header.version_flag = false;
    header.packet_sequence_number = 2;
    header.entropy_flag = false;
    header.fec_flag = false;
    header.fec_group = 0;

    QuicAckFrame ack(largest_received, QuicTime::Zero(), least_unacked);
    QuicCongestionFeedbackFrame feedback;
    feedback.type = kTCP;
    feedback.tcp.accumulated_number_of_lost_packets = 0;
    feedback.tcp.receive_window = 16000;

    QuicFramer framer(kQuicVersion1, QuicTime::Zero(), false);
    QuicFrames frames;
    frames.push_back(QuicFrame(&ack));
    frames.push_back(QuicFrame(&feedback));
    scoped_ptr<QuicPacket> packet(
        framer.ConstructFrameDataPacket(header, frames).packet);
    return scoped_ptr<QuicEncryptedPacket>(framer.EncryptPacket(
        ENCRYPTION_NONE, header.packet_sequence_number, *packet));
  }

  // Returns a newly created packet to send congestion feedback data.
  scoped_ptr<QuicEncryptedPacket> ConstructFeedbackPacket(
      QuicPacketSequenceNumber sequence_number) {
    QuicPacketHeader header;
    header.public_header.guid = 0xDEADBEEF;
    header.public_header.reset_flag = false;
    header.public_header.version_flag = false;
    header.packet_sequence_number = sequence_number;
    header.entropy_flag = false;
    header.fec_flag = false;
    header.fec_group = 0;

    QuicCongestionFeedbackFrame frame;
    frame.type = kTCP;
    frame.tcp.accumulated_number_of_lost_packets = 0;
    frame.tcp.receive_window = 16000;

    return scoped_ptr<QuicEncryptedPacket>(
        ConstructPacket(header, QuicFrame(&frame)));
  }

  scoped_ptr<QuicEncryptedPacket> ConstructPacket(
      const QuicPacketHeader& header,
      const QuicFrame& frame) {
    QuicFramer framer(kQuicVersion1, QuicTime::Zero(), false);
    QuicFrames frames;
    frames.push_back(frame);
    scoped_ptr<QuicPacket> packet(
        framer.ConstructFrameDataPacket(header, frames).packet);
    return scoped_ptr<QuicEncryptedPacket>(framer.EncryptPacket(
        ENCRYPTION_NONE, header.packet_sequence_number, *packet));
  }

  MockHostResolver host_resolver_;
  DeterministicMockClientSocketFactory socket_factory_;
  MockCryptoClientStreamFactory crypto_client_stream_factory_;
  MockRandom random_generator_;
  MockClock* clock_;  // Owned by factory_.
  QuicStreamFactory factory_;
  HostPortProxyPair host_port_proxy_pair_;
  BoundNetLog net_log_;
  TestCompletionCallback callback_;
};

TEST_F(QuicStreamFactoryTest, CreateIfSessionExists) {
  EXPECT_EQ(NULL, factory_.CreateIfSessionExists(host_port_proxy_pair_,
                                                 net_log_).get());
}

TEST_F(QuicStreamFactoryTest, Create) {
  scoped_ptr<QuicEncryptedPacket> rst3(ConstructRstPacket(1, 3));
  scoped_ptr<QuicEncryptedPacket> rst5(ConstructRstPacket(2, 5));
  scoped_ptr<QuicEncryptedPacket> rst7(ConstructRstPacket(3, 7));
  MockWrite writes[] = {
    MockWrite(SYNCHRONOUS, rst3->data(), rst3->length(), 0),
    MockWrite(SYNCHRONOUS, rst5->data(), rst5->length(), 1),
    MockWrite(SYNCHRONOUS, rst7->data(), rst7->length(), 2),
  };
  MockRead reads[] = {
    MockRead(ASYNC, OK, 3)  // EOF
  };
  DeterministicSocketData socket_data(reads, arraysize(reads),
                                      writes, arraysize(writes));
  socket_factory_.AddSocketDataProvider(&socket_data);
  socket_data.StopAfter(3);

  QuicStreamRequest request(&factory_);
  EXPECT_EQ(ERR_IO_PENDING, request.Request(host_port_proxy_pair_, net_log_,
                                            callback_.callback()));

  EXPECT_EQ(OK, callback_.WaitForResult());
  scoped_ptr<QuicHttpStream> stream = request.ReleaseStream();
  EXPECT_TRUE(stream.get());

  // Will reset stream 3.
  stream = factory_.CreateIfSessionExists(host_port_proxy_pair_, net_log_);
  EXPECT_TRUE(stream.get());

  QuicStreamRequest request2(&factory_);
  EXPECT_EQ(OK, request2.Request(host_port_proxy_pair_, net_log_,
                                 callback_.callback()));
  stream = request2.ReleaseStream();  // Will reset stream 5.
  stream.reset();  // Will reset stream 7.

  socket_data.RunFor(1);

  EXPECT_TRUE(socket_data.at_read_eof());
  EXPECT_TRUE(socket_data.at_write_eof());
}

TEST_F(QuicStreamFactoryTest, CreateError) {
  DeterministicSocketData socket_data(NULL, 0, NULL, 0);
  socket_factory_.AddSocketDataProvider(&socket_data);

  host_resolver_.rules()->AddSimulatedFailure("www.google.com");

  QuicStreamRequest request(&factory_);
  EXPECT_EQ(ERR_IO_PENDING, request.Request(host_port_proxy_pair_, net_log_,
                                            callback_.callback()));

  EXPECT_EQ(ERR_NAME_NOT_RESOLVED, callback_.WaitForResult());

  EXPECT_TRUE(socket_data.at_read_eof());
  EXPECT_TRUE(socket_data.at_write_eof());
}

TEST_F(QuicStreamFactoryTest, CancelCreate) {
  scoped_ptr<QuicEncryptedPacket> rst3(ConstructRstPacket(1, 3));

  MockWrite writes[] = {
    MockWrite(SYNCHRONOUS, rst3->data(), rst3->length(), 0),
  };
  MockRead reads[] = {
    MockRead(ASYNC, OK, 1)  // EOF
  };
  DeterministicSocketData socket_data(reads, arraysize(reads),
                                      writes, arraysize(writes));
  socket_factory_.AddSocketDataProvider(&socket_data);
  {
    QuicStreamRequest request(&factory_);
    EXPECT_EQ(ERR_IO_PENDING, request.Request(host_port_proxy_pair_, net_log_,
                                              callback_.callback()));
  }

  socket_data.StopAfter(2);
  base::RunLoop run_loop;
  run_loop.RunUntilIdle();

  scoped_ptr<QuicHttpStream> stream(
      factory_.CreateIfSessionExists(host_port_proxy_pair_, net_log_));
  EXPECT_TRUE(stream.get());
  stream.reset();

  socket_data.RunFor(1);

  EXPECT_TRUE(socket_data.at_read_eof());
  EXPECT_TRUE(socket_data.at_write_eof());
}

TEST_F(QuicStreamFactoryTest, CloseAllSessions) {
  scoped_ptr<QuicEncryptedPacket> rst3(ConstructRstPacket(1, 3));
  MockRead reads[] = {
    MockRead(ASYNC, 0, 0)  // EOF
  };
  DeterministicSocketData socket_data(reads, arraysize(reads),
                                      NULL, 0);
  socket_factory_.AddSocketDataProvider(&socket_data);
  socket_data.StopAfter(1);

  MockWrite writes2[] = {
    MockWrite(SYNCHRONOUS, rst3->data(), rst3->length(), 0),
  };
  MockRead reads2[] = {
    MockRead(ASYNC, 0, 1)  // EOF
  };
  DeterministicSocketData socket_data2(reads2, arraysize(reads2),
                                        writes2, arraysize(writes2));
  socket_factory_.AddSocketDataProvider(&socket_data2);
  socket_data2.StopAfter(1);

  QuicStreamRequest request(&factory_);
  EXPECT_EQ(ERR_IO_PENDING, request.Request(host_port_proxy_pair_, net_log_,
                                            callback_.callback()));

  EXPECT_EQ(OK, callback_.WaitForResult());
  scoped_ptr<QuicHttpStream> stream = request.ReleaseStream();

  // Close the session and verify that stream saw the error.
  factory_.CloseAllSessions(ERR_INTERNET_DISCONNECTED);
  EXPECT_EQ(ERR_INTERNET_DISCONNECTED,
            stream->ReadResponseHeaders(callback_.callback()));

  // Now attempting to request a stream to the same origin should create
  // a new session.

  QuicStreamRequest request2(&factory_);
  EXPECT_EQ(ERR_IO_PENDING, request2.Request(host_port_proxy_pair_, net_log_,
                                             callback_.callback()));

  EXPECT_EQ(OK, callback_.WaitForResult());
  stream = request2.ReleaseStream();
  stream.reset();  // Will reset stream 3.

  socket_data2.RunFor(1);

  EXPECT_TRUE(socket_data.at_read_eof());
  EXPECT_TRUE(socket_data.at_write_eof());
  EXPECT_TRUE(socket_data2.at_read_eof());
  EXPECT_TRUE(socket_data2.at_write_eof());
}

TEST_F(QuicStreamFactoryTest, OnIPAddressChanged) {
  scoped_ptr<QuicEncryptedPacket> rst3(ConstructRstPacket(1, 3));
  MockRead reads[] = {
    MockRead(ASYNC, 0, 0)  // EOF
  };
  DeterministicSocketData socket_data(reads, arraysize(reads),
                                      NULL, 0);
  socket_factory_.AddSocketDataProvider(&socket_data);
  socket_data.StopAfter(1);

  MockWrite writes2[] = {
    MockWrite(SYNCHRONOUS, rst3->data(), rst3->length(), 0),
  };
  MockRead reads2[] = {
    MockRead(ASYNC, 0, 1)  // EOF
  };
  DeterministicSocketData socket_data2(reads2, arraysize(reads2),
                                        writes2, arraysize(writes2));
  socket_factory_.AddSocketDataProvider(&socket_data2);
  socket_data2.StopAfter(1);

  QuicStreamRequest request(&factory_);
  EXPECT_EQ(ERR_IO_PENDING, request.Request(host_port_proxy_pair_, net_log_,
                                            callback_.callback()));

  EXPECT_EQ(OK, callback_.WaitForResult());
  scoped_ptr<QuicHttpStream> stream = request.ReleaseStream();

  // Change the IP address and verify that stream saw the error.
  factory_.OnIPAddressChanged();
  EXPECT_EQ(ERR_NETWORK_CHANGED,
            stream->ReadResponseHeaders(callback_.callback()));

  // Now attempting to request a stream to the same origin should create
  // a new session.

  QuicStreamRequest request2(&factory_);
  EXPECT_EQ(ERR_IO_PENDING, request2.Request(host_port_proxy_pair_, net_log_,
                                             callback_.callback()));

  EXPECT_EQ(OK, callback_.WaitForResult());
  stream = request2.ReleaseStream();
  stream.reset();  // Will reset stream 3.

  socket_data2.RunFor(1);

  EXPECT_TRUE(socket_data.at_read_eof());
  EXPECT_TRUE(socket_data.at_write_eof());
  EXPECT_TRUE(socket_data2.at_read_eof());
  EXPECT_TRUE(socket_data2.at_write_eof());
}

}  // namespace test
}  // namespace net
