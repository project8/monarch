/*
 * M3HDF5Mutex.hh
 *
 *  Created on: Jul 17, 2019
 *      Author: N.S. Oblath
 */

#ifndef M3HDF5MUTEX_HH_
#define M3HDF5MUTEX_HH_

#include "M3Constants.hh"
#include "M3MemberVariable.hh"

#include "singleton.hh"

namespace monarch3
{

    class M3_API HDF5Mutex : public scarab::singleton< HDF5Mutex >
    {
        public:
            M3MEMBERVARIABLE_SHARED_PTR( std::recursive_mutex, Mutex );

        private:
            friend class scarab::singleton< HDF5Mutex >;
            friend class scarab::destroyer< HDF5Mutex >;

            HDF5Mutex();
            virtual ~HDF5Mutex();
    };

} /* namespace monarch3 */

#endif /* M3HDF5MUTEX_HH_ */
