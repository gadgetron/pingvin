#pragma once

/** TODO Joe: Heavy cleanup needed vvvvv */

// #include <mrd/types.h>
// #include "ismrmrd/ismrmrd.h"
// #include "ismrmrd/waveform.h"
// #include "ismrmrd/meta.h"
// #include <vector>
// #include <set>
// #include "hoNDArray.h"
#include "Types.h"
// #include <io/adapt_struct.h>


namespace Gadgetron 
{

    /**
      This is a list of lables of the coordinates described in the ISMRMRD acquisition header.

      It is useful for accumulators and triggers and for labeling the storage used in
      the @IsmrmrdAcquisitionBucket and @IsmrmrdDataBuffered structures. 

   */
  //   enum IsmrmrdCONDITION {
	// KSPACE_ENCODE_STEP_1,
	// KSPACE_ENCODE_STEP_2,
	// AVERAGE,
	// SLICE,
	// CONTRAST,
	// PHASE,
	// REPETITION,
	// SET,
	// SEGMENT,
	// USER_0,
	// USER_1,
	// USER_2,
	// USER_3,
	// USER_4,
	// USER_5,
	// USER_6,
	// USER_7,
  //   N_ACQUISITIONS,
	// NONE
  //     };



    // define the dimensions of ISMRMRD
    // enum IsmrmrdDIM
    // {
    //     DIM_ReadOut = 32,
    //     DIM_Encoding1,
    //     DIM_Channel,
    //     DIM_Slice,
    //     DIM_Encoding2,
    //     DIM_Contrast,
    //     DIM_Phase,
    //     DIM_Repetition,
    //     DIM_Set,
    //     DIM_Segment,
    //     DIM_Average,
    //     DIM_other1,
    //     DIM_other2,
    //     DIM_other3,
    //     DIM_NONE
    // };


    // --------------------------------------------------------------------------
    /// define the calibration mode of ISMRMRD
    // --------------------------------------------------------------------------
    // enum ismrmrdCALIBMODE
    // {
    //     ISMRMRD_embedded,
    //     ISMRMRD_interleaved,
    //     ISMRMRD_separate,
    //     ISMRMRD_external,
    //     ISMRMRD_other,
    //     ISMRMRD_noacceleration
    // };

  // struct SamplingLimit
  // {
  //   uint16_t min_ = 0;
  //   uint16_t center_ = 0;
  //   uint16_t max_ = 0;
  // };
  
  // struct SamplingDescription
  // {
  //   std::array<float,3> encoded_FOV_ = {0,0,0};
  //   std::array<float,3> recon_FOV_ = {0,0,0};
    
  //   std::array<uint32_t ,3> encoded_matrix_ = {0,0,0};
  //   std::array<uint32_t ,3> recon_matrix_ = {0,0,0};
    
  //   // sampled range along RO, E1, E2 (for asymmetric echo and partial fourier)
  //   // min, max and center
  //   std::array<SamplingLimit, 3> sampling_limits_;
  // };
  
  // struct IsmrmrdDataBuffered
  // {
  // public:
  //   //7D, fixed order [E0, E1, E2, CHA, N, S, LOC]
  //   hoNDArray< std::complex<float> > data_;
    
  //   //7D, fixed order [TRAJ, E0, E1, E2, N, S, LOC]
  //   Core::optional<hoNDArray<float>> trajectory_;

  //   // 6D, density weights [E0, E1, E2, N, S, LOC]
  //   Core::optional<hoNDArray<float> > density_;

  //   //5D, fixed order [E1, E2, N, S, LOC]
  //   // hoNDArray< ISMRMRD::AcquisitionHeader > headers_;
  //   /** TODO Joe: This is a temporary hack */
  //   hoNDArray< mrd::AcquisitionHeader > headers_;

  //   mrd::SamplingDescription sampling_;
  // };

  // using IsmrmrdDataBuffered = mrd::BufferedData;
  

  /**
     This class is used to group a sub-unit of the data that would feed into a reconstruction. 
   */
  // struct IsmrmrdReconBit
  // {
  // public:
  //   IsmrmrdDataBuffered data_;
  //   Core::optional<IsmrmrdDataBuffered> ref_;
  // };
  // using IsmrmrdReconBit = mrd::ReconBit;

  /**
     This class is used to store a unit of data that would feed into a reconstruction. 
   */
  // struct IsmrmrdReconData
  // {
  // public:
  //   std::vector<IsmrmrdReconBit> rbit_;
  // };
  // using IsmrmrdReconData = mrd::ReconData;

  /**
     This class is used to store an array of reconstructed data. 
   */
  // struct IsmrmrdImageArray
  // {
  //   //7D, fixed order [X, Y, Z, CHA, N, S, LOC]
  //   hoNDArray< std::complex<float> > data_;
    
  //   //3D, fixed order [N, S, LOC]
  //   // hoNDArray< ISMRMRD::ImageHeader > headers_;
  //   /** TODO Joe: This is a temporary hack */
  //   hoNDArray< mrd::ImageHeader > headers_;
    
  //   //3D, fixed order [N, S, LOC]
  //   //This element is optional (length is 0 if not present)
  //   std::vector< mrd::ImageMeta > meta_;

  //   // wave form
  //   Core::optional<std::vector<mrd::WaveformUint32>> waveform_;

  //   // acquisition header, [Y, Z, N, S, LOC]
  //   // Core::optional<hoNDArray< mrd::AcquisitionHeader >> acq_headers_;
  // };

  // using IsmrmrdImageArray = mrd::ImageArray;

using ReconBit = mrd::ReconBit;
using ReconData = mrd::ReconData;
using ImageArray = mrd::ImageArray;
using DataBuffered = mrd::BufferedData;
using AcquisitionBucket = mrd::AcquisitionBucket;
using AcquisitionBucketStats = mrd::AcquisitionBucketStats;

void add_stats_to_bucket(mrd::AcquisitionBucketStats& stats, const mrd::AcquisitionHeader& header);
void add_acquisition_to_bucket(mrd::AcquisitionBucket& bucket, mrd::Acquisition acq);

}

// GADGETRON_ADAPT_STRUCT(Gadgetron::ImageArray, GADGETRON_ACCESS_ELEMENT(data_), GADGETRON_ACCESS_ELEMENT(headers_), GADGETRON_ACCESS_ELEMENT(meta_), GADGETRON_ACCESS_ELEMENT(waveform_), GADGETRON_ACCESS_ELEMENT(acq_headers_))
