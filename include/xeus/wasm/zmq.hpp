#ifndef XEUS_ZQM_HPP
#define XEUS_ZQM_HPP

#include <deque>
#include <sstream>
#include <iomanip>
#include <type_traits>
#include <vector>

namespace zmq
{


enum class socket_type : int
{
    req,
    rep,
    dealer,
    router,
    pub,
    sub,
    xpub,
    xsub,
    push,
    pull
};

enum class send_flags : int
{
    none = 0,
    dontwait =1,
    sndmore = 2
};

class const_buffer
{
};

namespace sockopt
{

    constexpr int linger = 0;
// There are two types of options,
// integral type with known compiler time size (int, bool, int64_t, uint64_t)
// and arrays with dynamic size (strings, binary data).

// BoolUnit: if true accepts values of type bool (but passed as T into libzmq)
template<int Opt, class T, bool BoolUnit = false> struct integral_option
{
};

// NullTerm:
// 0: binary data
// 1: null-terminated string (`getsockopt` size includes null)
// 2: binary (size 32) or Z85 encoder string of size 41 (null included)
template<int Opt, int NullTerm = 1> struct array_option
{
};

}

struct context_t
{};

struct socket_t
{
    socket_t(context_t &context_, int type_){}
    socket_t(context_t &context_, socket_type type_){}


    template<class ... args >
    void set(args && ... )
    {
    }

    template<class ... args >
    std::size_t send(args && ... )
    {
        return 0;
    }
    template<class ... args >
    std::size_t recv(args && ... )
    {
        return 0;
    }
    
};

class message_t
{
public:
    message_t()
    : m_data(){
    }

    message_t(const void * src, std::size_t size)
    : m_data(static_cast<const uint8_t *>(src),static_cast<const uint8_t *>(src) + size)
    {   

    }
    explicit message_t(size_t size_)
    : m_data(size_)
    {

    }


    template<class ForwardIter> 
    message_t(ForwardIter first, ForwardIter last)
    {
        typedef typename std::iterator_traits<ForwardIter>::value_type value_t;

        //assert(std::distance(first, last) >= 0);
        std:: size_t size_ =
          static_cast<size_t>(std::distance(first, last)) * sizeof(value_t);

        m_data.resize(size_);
        std::copy(first, last, data<value_t>());
    }


    std::size_t size()const
    {
        return m_data.size();
    }


    void *data() { 
        return static_cast<void*>( m_data.data()); 
    }

    const void *data() const
    {
        return static_cast<const void*>( m_data.data()); 
    }

    template<class T>
    T * data()
    {
        return reinterpret_cast<T*>(m_data.data());
    }

    template<class T>
    const T * data() const
    {
        return reinterpret_cast<const T*>(m_data.data());
    }

    bool operator==(const message_t &other) const 
    {
        const size_t my_size = size();
        return my_size == other.size() && 0 == memcmp(data(), other.data(), my_size);
    }

    bool operator!=(const message_t &other) const 
    {
        return !(*this == other);
    }

private:

    std::vector<uint8_t> m_data;
};



class multipart_t
{
  private:
    std::deque<message_t> m_parts;

  public:
    typedef std::deque<message_t>::value_type value_type;

    typedef std::deque<message_t>::iterator iterator;
    typedef std::deque<message_t>::const_iterator const_iterator;

    typedef std::deque<message_t>::reverse_iterator reverse_iterator;
    typedef std::deque<message_t>::const_reverse_iterator const_reverse_iterator;

    // Default constructor
    multipart_t() {}

    // // Construct from socket receive
    // multipart_t(socket_ref socket) { recv(socket); }

    // Construct from memory block
    multipart_t(const void *src, size_t size) { addmem(src, size); }

    // Construct from string
    multipart_t(const std::string &string) { addstr(string); }

    // Construct from message part
    multipart_t(message_t &&message) { add(std::move(message)); }

    // Move constructor
    multipart_t(multipart_t &&other) { m_parts = std::move(other.m_parts); }

    // Move assignment operator
    multipart_t &operator=(multipart_t &&other)
    {
        m_parts = std::move(other.m_parts);
        return *this;
    }

    // Destructor
    virtual ~multipart_t() { clear(); }

    message_t &operator[](size_t n) { return m_parts[n]; }

    const message_t &operator[](size_t n) const { return m_parts[n]; }

    message_t &at(size_t n) { return m_parts.at(n); }

    const message_t &at(size_t n) const { return m_parts.at(n); }

    iterator begin() { return m_parts.begin(); }

    const_iterator begin() const { return m_parts.begin(); }

    const_iterator cbegin() const { return m_parts.cbegin(); }

    reverse_iterator rbegin() { return m_parts.rbegin(); }

    const_reverse_iterator rbegin() const { return m_parts.rbegin(); }

    iterator end() { return m_parts.end(); }

    const_iterator end() const { return m_parts.end(); }

    const_iterator cend() const { return m_parts.cend(); }

    reverse_iterator rend() { return m_parts.rend(); }

    const_reverse_iterator rend() const { return m_parts.rend(); }

    // Delete all parts
    void clear() { m_parts.clear(); }

    // Get number of parts
    size_t size() const { return m_parts.size(); }

    // Check if number of parts is zero
    bool empty() const { return m_parts.empty(); }

//     // Receive multipart message from socket
//     bool recv(socket_ref socket, int flags = 0)
//     {
//         clear();
//         bool more = true;
//         while (more) {
//             message_t message;
// #ifdef ZMQ_CPP11
//             if (!socket.recv(message, static_cast<recv_flags>(flags)))
//                 return false;
// #else
//             if (!socket.recv(&message, flags))
//                 return false;
// #endif
//             more = message.more();
//             add(std::move(message));
//         }
//         return true;
//     }

//     // Send multipart message to socket
//     bool send(socket_ref socket, int flags = 0)
//     {
//         flags &= ~(ZMQ_SNDMORE);
//         bool more = size() > 0;
//         while (more) {
//             message_t message = pop();
//             more = size() > 0;
// #ifdef ZMQ_CPP11
//             if (!socket.send(message, static_cast<send_flags>(
//                                         (more ? ZMQ_SNDMORE : 0) | flags)))
//                 return false;
// #else
//             if (!socket.send(message, (more ? ZMQ_SNDMORE : 0) | flags))
//                 return false;
// #endif
//         }
//         clear();
//         return true;
//     }

    // Concatenate other multipart to front
    void prepend(multipart_t &&other)
    {
        while (!other.empty())
            push(other.remove());
    }

    // Concatenate other multipart to back
    void append(multipart_t &&other)
    {
        while (!other.empty())
            add(other.pop());
    }

    // Push memory block to front
    void pushmem(const void *src, size_t size)
    {
        m_parts.push_front(message_t(src, size));
    }

    // Push memory block to back
    void addmem(const void *src, size_t size)
    {
        m_parts.push_back(message_t(src, size));
    }

    // Push string to front
    void pushstr(const std::string &string)
    {
        m_parts.push_front(message_t(string.data(), string.size()));
    }

    // Push string to back
    void addstr(const std::string &string)
    {
        m_parts.push_back(message_t(string.data(), string.size()));
    }

    // Push type (fixed-size) to front
    template<typename T> void pushtyp(const T &type)
    {
        static_assert(!std::is_same<T, std::string>::value,
                      "Use pushstr() instead of pushtyp<std::string>()");
        m_parts.push_front(message_t(&type, sizeof(type)));
    }

    // Push type (fixed-size) to back
    template<typename T> void addtyp(const T &type)
    {
        static_assert(!std::is_same<T, std::string>::value,
                      "Use addstr() instead of addtyp<std::string>()");
        m_parts.push_back(message_t(&type, sizeof(type)));
    }

    // Push message part to front
    void push(message_t &&message) { m_parts.push_front(std::move(message)); }

    // Push message part to back
    void add(message_t &&message) { m_parts.push_back(std::move(message)); }

    // Alias to allow std::back_inserter()
    void push_back(message_t &&message) { m_parts.push_back(std::move(message)); }

    // Pop string from front
    std::string popstr()
    {
        std::string string(m_parts.front().data<char>(), m_parts.front().size());
        m_parts.pop_front();
        return string;
    }

    // Pop type (fixed-size) from front
    template<typename T> T poptyp()
    {
        static_assert(!std::is_same<T, std::string>::value,
                      "Use popstr() instead of poptyp<std::string>()");
        if (sizeof(T) != m_parts.front().size())
            throw std::runtime_error(
              "Invalid type, size does not match the message size");
        T type = *m_parts.front().data<T>();
        m_parts.pop_front();
        return type;
    }

    // Pop message part from front
    message_t pop()
    {
        message_t message = std::move(m_parts.front());
        m_parts.pop_front();
        return message;
    }

    // Pop message part from back
    message_t remove()
    {
        message_t message = std::move(m_parts.back());
        m_parts.pop_back();
        return message;
    }

    // get message part from front
    const message_t &front() { return m_parts.front(); }

    // get message part from back
    const message_t &back() { return m_parts.back(); }

    // Get pointer to a specific message part
    const message_t *peek(size_t index) const { return &m_parts[index]; }

    // Get a string copy of a specific message part
    std::string peekstr(size_t index) const
    {
        std::string string(m_parts[index].data<char>(), m_parts[index].size());
        return string;
    }

    // Peek type (fixed-size) from front
    template<typename T> T peektyp(size_t index) const
    {
        static_assert(!std::is_same<T, std::string>::value,
                      "Use peekstr() instead of peektyp<std::string>()");
        if (sizeof(T) != m_parts[index].size())
            throw std::runtime_error(
              "Invalid type, size does not match the message size");
        T type = *m_parts[index].data<T>();
        return type;
    }

    // Create multipart from type (fixed-size)
    template<typename T> static multipart_t create(const T &type)
    {
        multipart_t multipart;
        multipart.addtyp(type);
        return multipart;
    }

    // Copy multipart
    multipart_t clone() const
    {
        multipart_t multipart;
        for (size_t i = 0; i < size(); i++)
            multipart.addmem(m_parts[i].data(), m_parts[i].size());
        return multipart;
    }

    // Dump content to string
    std::string str() const
    {
        std::stringstream ss;
        for (size_t i = 0; i < m_parts.size(); i++) {
            const unsigned char *data = m_parts[i].data<unsigned char>();
            size_t size = m_parts[i].size();

            // Dump the message as text or binary
            bool isText = true;
            for (size_t j = 0; j < size; j++) {
                if (data[j] < 32 || data[j] > 127) {
                    isText = false;
                    break;
                }
            }
            ss << "\n[" << std::dec << std::setw(3) << std::setfill('0') << size
               << "] ";
            if (size >= 1000) {
                ss << "... (too big to print)";
                continue;
            }
            for (size_t j = 0; j < size; j++) {
                if (isText)
                    ss << static_cast<char>(data[j]);
                else
                    ss << std::hex << std::setw(2) << std::setfill('0')
                       << static_cast<short>(data[j]);
            }
        }
        return ss.str();
    }

    // Check if equal to other multipart
    bool equal(const multipart_t *other) const
    {
        if (size() != other->size())
            return false;
        for (size_t i = 0; i < size(); i++)
            if (*peek(i) != *other->peek(i))
                return false;
        return true;
    }

#ifdef ZMQ_CPP11

    // Return single part message_t encoded from this multipart_t.
    message_t encode() const { return zmq::encode(*this); }

    // Decode encoded message into multiple parts and append to self.
    void decode_append(const message_t &encoded)
    {
        zmq::decode(encoded, std::back_inserter(*this));
    }

    // Return a new multipart_t containing the decoded message_t.
    static multipart_t decode(const message_t &encoded)
    {
        multipart_t tmp;
        zmq::decode(encoded, std::back_inserter(tmp));
        return tmp;
    }

#endif

  private:
    // Disable implicit copying (moving is more efficient)
    multipart_t(const multipart_t &other) = delete;
    void operator=(const multipart_t &other) = delete;
}; // class multipart_t


}

#endif