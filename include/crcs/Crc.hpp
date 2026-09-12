#ifndef COMMON_CRCS_INCLUDE_CRCS_CRC_HPP
#define COMMON_CRCS_INCLUDE_CRCS_CRC_HPP

#include <cstdint>
#include <typeConversion/Bits.hpp>

namespace Crcs {
    using namespace Utils;

    /// @brief Base class for all CRCs, provides common functions and interface used to implement the specific CRCs
    /// @tparam T The underlying type of the CRC result
    /// @tparam LOOKUP_SIZE THe number of entries in the lookup table used to accelerate the CRC generation .
    /// @tparam DERIVED The actual implementation type. Used to generate the correct CRC from the stream templates
    template<typename T, uint32_t LOOKUP_SIZE, typename DERIVED>
    class BaseCrc {
    public:
        static_assert(isPower2(LOOKUP_SIZE));
        static constexpr uint32_t bitSlice = computeBitSize(LOOKUP_SIZE);
        static constexpr uint32_t mask = generateMask(bitSlice);
        static constexpr uint32_t slicesPerByte = computeSlicesPerByte(bitSlice);
        /// @brief Holds the bit/byte slice lookup table
        struct Lookup {
            T lookup[LOOKUP_SIZE] = {};
        };

        /// @brief Create a CRC with the correct initialisation of the CRC register.
        BaseCrc() = default;
        virtual ~BaseCrc() = default;

        ///
        /// @brief Compute the CRC of a steam of data.
        /// @tparam STREAM Needs to implement Read Stream functionality. Needs to be compatible with the free function
        ///                ::read<uint8_t>(data)
        /// @param data the stream that the bytes for the CRC are read from
        /// @param length the number of bytes to read fro the stream and compute the CRC32 over.
        /// @return The CRC 8/16 or 32 buts depending on the CRC implementation
        ///
        template<typename STREAM>
        static T computeCrc(STREAM &data, uint32_t length);
        ///
        /// @brief Compute the CRC of a steam of data.
        /// @tparam STREAM Needs to implement Read Stream functionality. Needs to be compatible with the free function
        ///                ::read<uint8_t>(data)
        /// @param data the stream that the bytes for the CRC are read from
        /// @param length the number of bytes to read fro the stream and compute the CRC32 over.
        /// @return The CRC 8/16 or 32 buts depending on the CRC implementation
        ///
        template<typename STREAM>
        static T computeCrc(STREAM &&data, uint32_t length);

        /// @brief Add a 32 bit word to the CRC LSB first.
        void write32(uint32_t data);

        /// @brief Add a 16 bit word to the CRC LSB first.
        void write16(uint32_t data);

        template <typename B>
        void writeBytes(B buffer);

        /// @brief add a single byte to the CRC
        virtual void write8(uint32_t data)=0;

        /// @brief Return the current value for the CRC. Will perform the correct close off for the CRC but does not
        /// modify the underlying CRC register so the data can be used for more computations.
        [[nodiscard]] virtual T getCrc() const=0;

    };

    /// @brief  Base type for all reflected CRCs When generation the CRC the msb is processed first.
    /// @tparam T The underlying type for the CRC result
    /// @tparam POLY The polynomial for the CRC. Needs to be reversed
    /// @tparam LOOKUP_SIZE  The number of entries in the lookup table
    /// @tparam INITIAL_VALUE  THe initial value for the CRC register.
    /// @tparam FINAL_XOR  The final value that the CRC register is XOR'ed with when generating the CRC result.
    ///
    template<typename T, uint32_t POLY, uint32_t LOOKUP_SIZE, uint32_t INITIAL_VALUE, uint32_t FINAL_XOR>
    class ReflectedCrc : public BaseCrc<T, LOOKUP_SIZE, ReflectedCrc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>> {
        using Base = BaseCrc<T, LOOKUP_SIZE, ReflectedCrc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>>;

        /// @brief Generate a CRC table of partial results to speed up computations.
        /// @return THe generated table of N entries.
        static constexpr typename Base::Lookup makeCrcTable() {
            typename Base::Lookup newTable = {{}};
            for (uint32_t n = 0U; n < LOOKUP_SIZE; ++n) {
                uint32_t crcSlice = n;
                for (uint32_t b = 0U; b < Base::bitSlice; ++b) {
                    if ((crcSlice & 0x1U) == 0x01U) {
                        crcSlice = POLY ^ (crcSlice >> 1U);
                    } else {
                        crcSlice = crcSlice >> 1U;
                    }
                }
                newTable.lookup[n] = static_cast<T>(crcSlice);
            }
            return newTable;
        }

        /// @brief The precomputed lookup table used to speed up the CRC computation by processing N buts at a time.
        static constexpr typename Base::Lookup crcTable = makeCrcTable();

        /// Holds intermediate CRC values whilst the bytes are being processed.

        T current;

    public:
        /// @brief create CRC generator of the given POLY and type with CRC register initialised with the correct
        /// starting value
        ReflectedCrc();

        /// @brief return the final result for the CRC. Xor ors the CRC register with the correct value but does not
        /// update it enabling the CRC to still continue on.
        [[nodiscard]] T getCrc() const override;

        /// @brief add a single byte to the CRC
        void write8(uint32_t data) override;


    protected:
        /// @brief add the next N bits from the current value to the CRC and compute the new CRC value
        void updateCrcSlice(uint32_t value);
    };


    /// @brief Base type for all CRCs that are not reflected. When generating the CRC the lsb is processed first.
    /// @tparam T The underlying type of the result 8/16/32 bit type
    /// @tparam POLY The polynomial for the CRC in standard format
    /// @tparam LOOKUP_SIZE The number of entries in the lookup table used to speed up the computation
    /// @tparam INITIAL_VALUE The initial value for the CRC register.
    /// @tparam FINAL_XOR THe final value XOR'ed to the running CRC register to get the CRC result.
    ///
    template<typename T, uint32_t POLY, uint32_t LOOKUP_SIZE, uint32_t INITIAL_VALUE, uint32_t FINAL_XOR>
    class Crc : public BaseCrc<T, LOOKUP_SIZE, ReflectedCrc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>> {
        using Base = BaseCrc<T, LOOKUP_SIZE, ReflectedCrc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>>;


        /// @brief  Builds a lookup table at compile time to speed up the computation of the CRC .
        ///         data is shifted in lsb first when generating the lookup values for the slices.
        static constexpr typename Base::Lookup makeCrcTable() {
            typename Base::Lookup newTable = {{}};
            constexpr uint32_t msBit = 0x1 << (sizeof(T)*8-1);
            constexpr uint32_t shift = sizeof(T)*8-Base::bitSlice;
            for (uint32_t n = 0U; n < LOOKUP_SIZE; ++n) {
                uint32_t crcSlice = n << shift;
                for (uint32_t b = 0U; b < Base::bitSlice; ++b) {
                    if ((crcSlice & msBit)!=0) {
                        crcSlice = POLY ^ (crcSlice << 1U);
                    } else {
                        crcSlice = crcSlice << 1U;
                    }
                }
                newTable.lookup[n] = static_cast<T>(crcSlice);
            }
            return newTable;
        }

        ///@brief the lookup table used to generate the speed up. There is a size trade off between speed and storage.
        static constexpr typename Base::Lookup crcTable = makeCrcTable();

        /// @brief The current intermediate value for the CRC.
        T current;

    public:
        /// @brief Create a CRC generator that has the intermediate CRC initialised to the starting value
        Crc();

        /// @brief return the fina CRC value. THe intermediate value is XOR'ed with the correct fina value
        ///               and returned. However the intermediate value is not updated so that calling this function
        ///               multiple times is allowed.
        [[nodiscard]] T getCrc() const override;

        /// @brief Add a single byte to the intermediate CRC. shifting in the bits lsb first.
        void write8(uint32_t data) override;

    protected:
      /// @brief Add a single N bit slice to the intermediate CRC. shifting in the bits lsb first. The slice is
      /// determined by the size of the lookup table.
      void updateCrcSlice(uint32_t value);
    };

    // BaseCrc
    // -------

    /// @brief write 32 bits lsb First
    template<typename T, uint32_t LOOKUP_SIZE, typename DERIVED>
    void BaseCrc<T, LOOKUP_SIZE, DERIVED>::write32(uint32_t data) {
        write8(data);
        write8(data >> 8U);
        write8(data >> 16U);
        write8(data >> 24U);
    }

  /// @brief write 32 bits lsb First
  template<typename T, uint32_t LOOKUP_SIZE, typename DERIVED>
  void BaseCrc<T, LOOKUP_SIZE, DERIVED>::write16(uint32_t data) {
    write8(data);
    write8(data >> 8U);
  }




  template<typename T, uint32_t LOOKUP_SIZE, typename DERIVED>
    template<typename STREAM>
    T BaseCrc<T, LOOKUP_SIZE, DERIVED>::computeCrc(STREAM &data, uint32_t length) {
        DERIVED crc;
        while (length > 0) {
            uint8_t val=0;
            data.read(val);
            crc.write8(val);
            length -= 1U;
        }
        return crc.getCrc();
    }

    template<typename T, uint32_t LOOKUP_SIZE, typename DERIVED>
    template<typename STREAM>
    T BaseCrc<T, LOOKUP_SIZE, DERIVED>::computeCrc(STREAM &&data, uint32_t length) {
        return computeCrc(data, length);
    }

  template< typename T, uint32_t LOOKUP_SIZE, typename DERIVED >
  template< typename B >
  void BaseCrc<T, LOOKUP_SIZE, DERIVED>::writeBytes(B buffer) {
    for(uint8_t v :buffer){
      write8(v);
    }
  }

  // Crc
    // ---
    // Compute Crc shifting lsb in first

  template<typename T, uint32_t POLY, uint32_t LOOKUP_SIZE, uint32_t INITIAL_VALUE, uint32_t FINAL_XOR>
    Crc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>::Crc():
            Base(),
            current(INITIAL_VALUE) {
    }

    template<typename T, uint32_t POLY, uint32_t LOOKUP_SIZE, uint32_t INITIAL_VALUE, uint32_t FINAL_XOR>
    void Crc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>::updateCrcSlice(const uint32_t value) {
        uint32_t index = (current >> (sizeof(current)*8 - Base::bitSlice)) ^ (value & Base::mask);
        current = static_cast<T>(crcTable.lookup[index] ^ (current << Base::bitSlice));
    }

    template<typename T, uint32_t POLY, uint32_t LOOKUP_SIZE, uint32_t INITIAL_VALUE, uint32_t FINAL_XOR>
    T Crc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>::getCrc() const {
        return static_cast<T>(current ^ FINAL_XOR);
    }

    template<typename T, uint32_t POLY, uint32_t LOOKUP_SIZE, uint32_t INITIAL_VALUE, uint32_t FINAL_XOR>
    void Crc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>::write8(uint32_t data) {
        for (int i = Base::slicesPerByte; i>0;  --i) {
            updateCrcSlice(data>> (i-1)*Base::bitSlice);
        }
    }


    // ReflectedCrc
    // ------------
    // Compute Crc shifting msb in first
    template<typename T, uint32_t POLY, uint32_t LOOKUP_SIZE, uint32_t INITIAL_VALUE, uint32_t FINAL_XOR>
    ReflectedCrc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>::ReflectedCrc():
            Base(),
            current(INITIAL_VALUE) {
    }

    template<typename T, uint32_t POLY, uint32_t LOOKUP_SIZE, uint32_t INITIAL_VALUE, uint32_t FINAL_XOR>
    void ReflectedCrc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>::updateCrcSlice(const uint32_t value) {
        current = crcTable.lookup[(current ^ value) & Base::mask] ^ static_cast<T>( (current >> Base::bitSlice));
    }

    template<typename T, uint32_t POLY, uint32_t LOOKUP_SIZE, uint32_t INITIAL_VALUE, uint32_t FINAL_XOR>
    T ReflectedCrc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>::getCrc() const {
        return static_cast<T>(current ^ FINAL_XOR);
    }

    template<typename T, uint32_t POLY, uint32_t LOOKUP_SIZE, uint32_t INITIAL_VALUE, uint32_t FINAL_XOR>
    void ReflectedCrc<T, POLY, LOOKUP_SIZE, INITIAL_VALUE, FINAL_XOR>::write8(uint32_t data) {
        for (uint32_t i = 0; i < Base::slicesPerByte; ++i) {
            updateCrcSlice(data);
            data = data >> Base::bitSlice;
        }
    }
}

#endif
