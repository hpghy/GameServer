// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: proto/client_gate.proto

#ifndef PROTOBUF_INCLUDED_proto_2fclient_5fgate_2eproto
#define PROTOBUF_INCLUDED_proto_2fclient_5fgate_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/service.h>
#include <google/protobuf/unknown_field_set.h>
#include "proto/common.pb.h"
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_proto_2fclient_5fgate_2eproto 

namespace protobuf_proto_2fclient_5fgate_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[3];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_proto_2fclient_5fgate_2eproto
namespace proto {
class EncryptKey;
class EncryptKeyDefaultTypeInternal;
extern EncryptKeyDefaultTypeInternal _EncryptKey_default_instance_;
class EncryptKeyStr;
class EncryptKeyStrDefaultTypeInternal;
extern EncryptKeyStrDefaultTypeInternal _EncryptKeyStr_default_instance_;
class SessionSeed;
class SessionSeedDefaultTypeInternal;
extern SessionSeedDefaultTypeInternal _SessionSeed_default_instance_;
}  // namespace proto
namespace google {
namespace protobuf {
template<> ::proto::EncryptKey* Arena::CreateMaybeMessage<::proto::EncryptKey>(Arena*);
template<> ::proto::EncryptKeyStr* Arena::CreateMaybeMessage<::proto::EncryptKeyStr>(Arena*);
template<> ::proto::SessionSeed* Arena::CreateMaybeMessage<::proto::SessionSeed>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace proto {

// ===================================================================

class SessionSeed : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:proto.SessionSeed) */ {
 public:
  SessionSeed();
  virtual ~SessionSeed();

  SessionSeed(const SessionSeed& from);

  inline SessionSeed& operator=(const SessionSeed& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  SessionSeed(SessionSeed&& from) noexcept
    : SessionSeed() {
    *this = ::std::move(from);
  }

  inline SessionSeed& operator=(SessionSeed&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const SessionSeed& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const SessionSeed* internal_default_instance() {
    return reinterpret_cast<const SessionSeed*>(
               &_SessionSeed_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(SessionSeed* other);
  friend void swap(SessionSeed& a, SessionSeed& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline SessionSeed* New() const final {
    return CreateMaybeMessage<SessionSeed>(NULL);
  }

  SessionSeed* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<SessionSeed>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const SessionSeed& from);
  void MergeFrom(const SessionSeed& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(SessionSeed* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // uint64 seed = 1;
  void clear_seed();
  static const int kSeedFieldNumber = 1;
  ::google::protobuf::uint64 seed() const;
  void set_seed(::google::protobuf::uint64 value);

  // @@protoc_insertion_point(class_scope:proto.SessionSeed)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::uint64 seed_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_proto_2fclient_5fgate_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class EncryptKeyStr : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:proto.EncryptKeyStr) */ {
 public:
  EncryptKeyStr();
  virtual ~EncryptKeyStr();

  EncryptKeyStr(const EncryptKeyStr& from);

  inline EncryptKeyStr& operator=(const EncryptKeyStr& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  EncryptKeyStr(EncryptKeyStr&& from) noexcept
    : EncryptKeyStr() {
    *this = ::std::move(from);
  }

  inline EncryptKeyStr& operator=(EncryptKeyStr&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const EncryptKeyStr& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const EncryptKeyStr* internal_default_instance() {
    return reinterpret_cast<const EncryptKeyStr*>(
               &_EncryptKeyStr_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(EncryptKeyStr* other);
  friend void swap(EncryptKeyStr& a, EncryptKeyStr& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline EncryptKeyStr* New() const final {
    return CreateMaybeMessage<EncryptKeyStr>(NULL);
  }

  EncryptKeyStr* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<EncryptKeyStr>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const EncryptKeyStr& from);
  void MergeFrom(const EncryptKeyStr& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(EncryptKeyStr* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bytes encrypt_key_str = 1;
  void clear_encrypt_key_str();
  static const int kEncryptKeyStrFieldNumber = 1;
  const ::std::string& encrypt_key_str() const;
  void set_encrypt_key_str(const ::std::string& value);
  #if LANG_CXX11
  void set_encrypt_key_str(::std::string&& value);
  #endif
  void set_encrypt_key_str(const char* value);
  void set_encrypt_key_str(const void* value, size_t size);
  ::std::string* mutable_encrypt_key_str();
  ::std::string* release_encrypt_key_str();
  void set_allocated_encrypt_key_str(::std::string* encrypt_key_str);

  // @@protoc_insertion_point(class_scope:proto.EncryptKeyStr)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr encrypt_key_str_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_proto_2fclient_5fgate_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class EncryptKey : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:proto.EncryptKey) */ {
 public:
  EncryptKey();
  virtual ~EncryptKey();

  EncryptKey(const EncryptKey& from);

  inline EncryptKey& operator=(const EncryptKey& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  EncryptKey(EncryptKey&& from) noexcept
    : EncryptKey() {
    *this = ::std::move(from);
  }

  inline EncryptKey& operator=(EncryptKey&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const EncryptKey& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const EncryptKey* internal_default_instance() {
    return reinterpret_cast<const EncryptKey*>(
               &_EncryptKey_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  void Swap(EncryptKey* other);
  friend void swap(EncryptKey& a, EncryptKey& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline EncryptKey* New() const final {
    return CreateMaybeMessage<EncryptKey>(NULL);
  }

  EncryptKey* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<EncryptKey>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const EncryptKey& from);
  void MergeFrom(const EncryptKey& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(EncryptKey* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bytes random_header = 1;
  void clear_random_header();
  static const int kRandomHeaderFieldNumber = 1;
  const ::std::string& random_header() const;
  void set_random_header(const ::std::string& value);
  #if LANG_CXX11
  void set_random_header(::std::string&& value);
  #endif
  void set_random_header(const char* value);
  void set_random_header(const void* value, size_t size);
  ::std::string* mutable_random_header();
  ::std::string* release_random_header();
  void set_allocated_random_header(::std::string* random_header);

  // bytes encrypt_key = 2;
  void clear_encrypt_key();
  static const int kEncryptKeyFieldNumber = 2;
  const ::std::string& encrypt_key() const;
  void set_encrypt_key(const ::std::string& value);
  #if LANG_CXX11
  void set_encrypt_key(::std::string&& value);
  #endif
  void set_encrypt_key(const char* value);
  void set_encrypt_key(const void* value, size_t size);
  ::std::string* mutable_encrypt_key();
  ::std::string* release_encrypt_key();
  void set_allocated_encrypt_key(::std::string* encrypt_key);

  // bytes random_tail = 4;
  void clear_random_tail();
  static const int kRandomTailFieldNumber = 4;
  const ::std::string& random_tail() const;
  void set_random_tail(const ::std::string& value);
  #if LANG_CXX11
  void set_random_tail(::std::string&& value);
  #endif
  void set_random_tail(const char* value);
  void set_random_tail(const void* value, size_t size);
  ::std::string* mutable_random_tail();
  ::std::string* release_random_tail();
  void set_allocated_random_tail(::std::string* random_tail);

  // uint64 seed = 3;
  void clear_seed();
  static const int kSeedFieldNumber = 3;
  ::google::protobuf::uint64 seed() const;
  void set_seed(::google::protobuf::uint64 value);

  // @@protoc_insertion_point(class_scope:proto.EncryptKey)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr random_header_;
  ::google::protobuf::internal::ArenaStringPtr encrypt_key_;
  ::google::protobuf::internal::ArenaStringPtr random_tail_;
  ::google::protobuf::uint64 seed_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_proto_2fclient_5fgate_2eproto::TableStruct;
};
// ===================================================================

class IGateService_Stub;

class IGateService : public ::google::protobuf::Service {
 protected:
  // This class should be treated as an abstract interface.
  inline IGateService() {};
 public:
  virtual ~IGateService();

  typedef IGateService_Stub Stub;

  static const ::google::protobuf::ServiceDescriptor* descriptor();

  virtual void sesionSeedRequest(::google::protobuf::RpcController* controller,
                       const ::proto::None* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  virtual void encryptKeyRequest(::google::protobuf::RpcController* controller,
                       const ::proto::EncryptKeyStr* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  virtual void loginRequest(::google::protobuf::RpcController* controller,
                       const ::proto::LoginRequest* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  virtual void callEntityRpc(::google::protobuf::RpcController* controller,
                       const ::proto::RpcMessage* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);

  // implements Service ----------------------------------------------

  const ::google::protobuf::ServiceDescriptor* GetDescriptor();
  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  ::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response,
                  ::google::protobuf::Closure* done);
  const ::google::protobuf::Message& GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const;
  const ::google::protobuf::Message& GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(IGateService);
};

class IGateService_Stub : public IGateService {
 public:
  IGateService_Stub(::google::protobuf::RpcChannel* channel);
  IGateService_Stub(::google::protobuf::RpcChannel* channel,
                   ::google::protobuf::Service::ChannelOwnership ownership);
  ~IGateService_Stub();

  inline ::google::protobuf::RpcChannel* channel() { return channel_; }

  // implements IGateService ------------------------------------------

  void sesionSeedRequest(::google::protobuf::RpcController* controller,
                       const ::proto::None* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  void encryptKeyRequest(::google::protobuf::RpcController* controller,
                       const ::proto::EncryptKeyStr* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  void loginRequest(::google::protobuf::RpcController* controller,
                       const ::proto::LoginRequest* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  void callEntityRpc(::google::protobuf::RpcController* controller,
                       const ::proto::RpcMessage* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
 private:
  ::google::protobuf::RpcChannel* channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(IGateService_Stub);
};


// -------------------------------------------------------------------

class IClientService_Stub;

class IClientService : public ::google::protobuf::Service {
 protected:
  // This class should be treated as an abstract interface.
  inline IClientService() {};
 public:
  virtual ~IClientService();

  typedef IClientService_Stub Stub;

  static const ::google::protobuf::ServiceDescriptor* descriptor();

  virtual void sessionSeedReply(::google::protobuf::RpcController* controller,
                       const ::proto::None* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  virtual void encryptKeyReply(::google::protobuf::RpcController* controller,
                       const ::proto::None* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  virtual void loginReply(::google::protobuf::RpcController* controller,
                       const ::proto::LoginReply* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  virtual void callEntityRpc(::google::protobuf::RpcController* controller,
                       const ::proto::RpcMessage* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);

  // implements Service ----------------------------------------------

  const ::google::protobuf::ServiceDescriptor* GetDescriptor();
  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  ::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response,
                  ::google::protobuf::Closure* done);
  const ::google::protobuf::Message& GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const;
  const ::google::protobuf::Message& GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(IClientService);
};

class IClientService_Stub : public IClientService {
 public:
  IClientService_Stub(::google::protobuf::RpcChannel* channel);
  IClientService_Stub(::google::protobuf::RpcChannel* channel,
                   ::google::protobuf::Service::ChannelOwnership ownership);
  ~IClientService_Stub();

  inline ::google::protobuf::RpcChannel* channel() { return channel_; }

  // implements IClientService ------------------------------------------

  void sessionSeedReply(::google::protobuf::RpcController* controller,
                       const ::proto::None* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  void encryptKeyReply(::google::protobuf::RpcController* controller,
                       const ::proto::None* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  void loginReply(::google::protobuf::RpcController* controller,
                       const ::proto::LoginReply* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
  void callEntityRpc(::google::protobuf::RpcController* controller,
                       const ::proto::RpcMessage* request,
                       ::proto::None* response,
                       ::google::protobuf::Closure* done);
 private:
  ::google::protobuf::RpcChannel* channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(IClientService_Stub);
};


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// SessionSeed

// uint64 seed = 1;
inline void SessionSeed::clear_seed() {
  seed_ = GOOGLE_ULONGLONG(0);
}
inline ::google::protobuf::uint64 SessionSeed::seed() const {
  // @@protoc_insertion_point(field_get:proto.SessionSeed.seed)
  return seed_;
}
inline void SessionSeed::set_seed(::google::protobuf::uint64 value) {
  
  seed_ = value;
  // @@protoc_insertion_point(field_set:proto.SessionSeed.seed)
}

// -------------------------------------------------------------------

// EncryptKeyStr

// bytes encrypt_key_str = 1;
inline void EncryptKeyStr::clear_encrypt_key_str() {
  encrypt_key_str_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& EncryptKeyStr::encrypt_key_str() const {
  // @@protoc_insertion_point(field_get:proto.EncryptKeyStr.encrypt_key_str)
  return encrypt_key_str_.GetNoArena();
}
inline void EncryptKeyStr::set_encrypt_key_str(const ::std::string& value) {
  
  encrypt_key_str_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:proto.EncryptKeyStr.encrypt_key_str)
}
#if LANG_CXX11
inline void EncryptKeyStr::set_encrypt_key_str(::std::string&& value) {
  
  encrypt_key_str_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:proto.EncryptKeyStr.encrypt_key_str)
}
#endif
inline void EncryptKeyStr::set_encrypt_key_str(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  encrypt_key_str_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:proto.EncryptKeyStr.encrypt_key_str)
}
inline void EncryptKeyStr::set_encrypt_key_str(const void* value, size_t size) {
  
  encrypt_key_str_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:proto.EncryptKeyStr.encrypt_key_str)
}
inline ::std::string* EncryptKeyStr::mutable_encrypt_key_str() {
  
  // @@protoc_insertion_point(field_mutable:proto.EncryptKeyStr.encrypt_key_str)
  return encrypt_key_str_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* EncryptKeyStr::release_encrypt_key_str() {
  // @@protoc_insertion_point(field_release:proto.EncryptKeyStr.encrypt_key_str)
  
  return encrypt_key_str_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void EncryptKeyStr::set_allocated_encrypt_key_str(::std::string* encrypt_key_str) {
  if (encrypt_key_str != NULL) {
    
  } else {
    
  }
  encrypt_key_str_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), encrypt_key_str);
  // @@protoc_insertion_point(field_set_allocated:proto.EncryptKeyStr.encrypt_key_str)
}

// -------------------------------------------------------------------

// EncryptKey

// bytes random_header = 1;
inline void EncryptKey::clear_random_header() {
  random_header_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& EncryptKey::random_header() const {
  // @@protoc_insertion_point(field_get:proto.EncryptKey.random_header)
  return random_header_.GetNoArena();
}
inline void EncryptKey::set_random_header(const ::std::string& value) {
  
  random_header_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:proto.EncryptKey.random_header)
}
#if LANG_CXX11
inline void EncryptKey::set_random_header(::std::string&& value) {
  
  random_header_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:proto.EncryptKey.random_header)
}
#endif
inline void EncryptKey::set_random_header(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  random_header_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:proto.EncryptKey.random_header)
}
inline void EncryptKey::set_random_header(const void* value, size_t size) {
  
  random_header_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:proto.EncryptKey.random_header)
}
inline ::std::string* EncryptKey::mutable_random_header() {
  
  // @@protoc_insertion_point(field_mutable:proto.EncryptKey.random_header)
  return random_header_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* EncryptKey::release_random_header() {
  // @@protoc_insertion_point(field_release:proto.EncryptKey.random_header)
  
  return random_header_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void EncryptKey::set_allocated_random_header(::std::string* random_header) {
  if (random_header != NULL) {
    
  } else {
    
  }
  random_header_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), random_header);
  // @@protoc_insertion_point(field_set_allocated:proto.EncryptKey.random_header)
}

// bytes encrypt_key = 2;
inline void EncryptKey::clear_encrypt_key() {
  encrypt_key_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& EncryptKey::encrypt_key() const {
  // @@protoc_insertion_point(field_get:proto.EncryptKey.encrypt_key)
  return encrypt_key_.GetNoArena();
}
inline void EncryptKey::set_encrypt_key(const ::std::string& value) {
  
  encrypt_key_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:proto.EncryptKey.encrypt_key)
}
#if LANG_CXX11
inline void EncryptKey::set_encrypt_key(::std::string&& value) {
  
  encrypt_key_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:proto.EncryptKey.encrypt_key)
}
#endif
inline void EncryptKey::set_encrypt_key(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  encrypt_key_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:proto.EncryptKey.encrypt_key)
}
inline void EncryptKey::set_encrypt_key(const void* value, size_t size) {
  
  encrypt_key_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:proto.EncryptKey.encrypt_key)
}
inline ::std::string* EncryptKey::mutable_encrypt_key() {
  
  // @@protoc_insertion_point(field_mutable:proto.EncryptKey.encrypt_key)
  return encrypt_key_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* EncryptKey::release_encrypt_key() {
  // @@protoc_insertion_point(field_release:proto.EncryptKey.encrypt_key)
  
  return encrypt_key_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void EncryptKey::set_allocated_encrypt_key(::std::string* encrypt_key) {
  if (encrypt_key != NULL) {
    
  } else {
    
  }
  encrypt_key_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), encrypt_key);
  // @@protoc_insertion_point(field_set_allocated:proto.EncryptKey.encrypt_key)
}

// uint64 seed = 3;
inline void EncryptKey::clear_seed() {
  seed_ = GOOGLE_ULONGLONG(0);
}
inline ::google::protobuf::uint64 EncryptKey::seed() const {
  // @@protoc_insertion_point(field_get:proto.EncryptKey.seed)
  return seed_;
}
inline void EncryptKey::set_seed(::google::protobuf::uint64 value) {
  
  seed_ = value;
  // @@protoc_insertion_point(field_set:proto.EncryptKey.seed)
}

// bytes random_tail = 4;
inline void EncryptKey::clear_random_tail() {
  random_tail_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& EncryptKey::random_tail() const {
  // @@protoc_insertion_point(field_get:proto.EncryptKey.random_tail)
  return random_tail_.GetNoArena();
}
inline void EncryptKey::set_random_tail(const ::std::string& value) {
  
  random_tail_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:proto.EncryptKey.random_tail)
}
#if LANG_CXX11
inline void EncryptKey::set_random_tail(::std::string&& value) {
  
  random_tail_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:proto.EncryptKey.random_tail)
}
#endif
inline void EncryptKey::set_random_tail(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  random_tail_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:proto.EncryptKey.random_tail)
}
inline void EncryptKey::set_random_tail(const void* value, size_t size) {
  
  random_tail_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:proto.EncryptKey.random_tail)
}
inline ::std::string* EncryptKey::mutable_random_tail() {
  
  // @@protoc_insertion_point(field_mutable:proto.EncryptKey.random_tail)
  return random_tail_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* EncryptKey::release_random_tail() {
  // @@protoc_insertion_point(field_release:proto.EncryptKey.random_tail)
  
  return random_tail_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void EncryptKey::set_allocated_random_tail(::std::string* random_tail) {
  if (random_tail != NULL) {
    
  } else {
    
  }
  random_tail_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), random_tail);
  // @@protoc_insertion_point(field_set_allocated:proto.EncryptKey.random_tail)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace proto

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_proto_2fclient_5fgate_2eproto
