/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved. 

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/
#ifndef __itkCVTImageFilter_h
#define __itkCVTImageFilter_h

#include <vector>

#include "itkImage.h"
#include "itkProcessObject.h"
#include "itkIndex.h"
#include "itkContinuousIndex.h"
#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"

namespace itk
{
template < class TInputImage, class TOutputImage=TInputImage >
class ITK_EXPORT CVTImageFilter :   
  public ImageToImageFilter< TInputImage, TOutputImage >
{  
public:
  
  /** Standard class typedefs. */
  typedef CVTImageFilter                                  Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage>  Superclass;
  typedef SmartPointer<Self>                              Pointer;
  typedef SmartPointer<const Self>                        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  itkTypeMacro(CVTImageFilter, ImageToImageFilter);

  itkStaticConstMacro(ImageDimension, unsigned int,
                                      TInputImage::ImageDimension);
  
  typedef TInputImage                                     InputImageType;
  typedef typename InputImageType::PixelType              InputPixelType;

  typedef typename InputImageType::IndexType              IndexType;
  typedef ContinuousIndex<double, itkGetStaticConstMacro(ImageDimension)>
                                                          ContinuousIndexType;
  
  typedef TOutputImage                                    OutputImageType;
  typedef typename OutputImageType::PixelType             OutputPixelType;
  
  typedef typename InputImageType::RegionType             RegionType;
  typedef typename RegionType::SizeType                   SizeType;

  typedef std::vector<ContinuousIndexType>                PointArrayType;

  typedef enum {CVT_GRID, CVT_RANDOM, CVT_USER}           SamplingMethodEnum;
    
  /** */
  itkGetMacro(NumberOfCentroids, unsigned int);
  itkSetMacro(NumberOfCentroids, unsigned int);

  /** */
  itkGetMacro(InitialSamplingMethod, SamplingMethodEnum);
  itkSetMacro(InitialSamplingMethod, SamplingMethodEnum);

  /** */
  itkGetMacro(NumberOfSamples, unsigned int);
  itkSetMacro(NumberOfSamples, unsigned int);

  /** */
  itkGetMacro(NumberOfIterations, unsigned int);
  itkSetMacro(NumberOfIterations, unsigned int);

  /** */
  itkGetMacro(NumberOfIterationsPerBatch, unsigned int);
  itkSetMacro(NumberOfIterationsPerBatch, unsigned int);

  /** */
  itkGetMacro(NumberOfSamplesPerBatch, unsigned int);
  itkSetMacro(NumberOfSamplesPerBatch, unsigned int);

  /** */
  itkGetMacro(BatchSamplingMethod, SamplingMethodEnum);
  itkSetMacro(BatchSamplingMethod, SamplingMethodEnum);

  /** */
  PointArrayType * GetCentroids()
    { return & m_Centroids; }
  void SetCentroids(const PointArrayType * centroids);

  /** */
  itkGetMacro(Seed, long int);
  itkSetMacro(Seed, long int);


protected:
  CVTImageFilter();
  ~CVTImageFilter() {};

  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateInputRequestedRegion();
  void EnlargeOutputRequestedRegion(DataObject * output);
  void GenerateData();

  double ComputeIteration(double & energyDiff);
  void ComputeSample(PointArrayType * sample, unsigned int sampleSize,
                     SamplingMethodEnum samplingMethod);
  void ComputeClosest(const PointArrayType & sample,
                      const PointArrayType & centroids,
                      unsigned int * nearest);

private:
  CVTImageFilter(const Self&);
  void operator=(const Self&);
  
  typename OutputImageType::Pointer            m_OutputImage;

  typename InputImageType::ConstPointer        m_InputImage;

  unsigned int          m_NumberOfCentroids;
  PointArrayType        m_Centroids;

  double                m_InputImageMax;
  SizeType              m_InputImageSize;

  long int              m_Seed;
  itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer 
                        m_RandomGenerator;

  SamplingMethodEnum    m_InitialSamplingMethod;

  unsigned int          m_NumberOfSamples;
  unsigned int          m_NumberOfIterations;

  SamplingMethodEnum    m_BatchSamplingMethod;
  unsigned int          m_NumberOfIterationsPerBatch;
  unsigned int          m_NumberOfSamplesPerBatch;

}; // end of class 

}// end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCVTImageFilter.txx"
#endif   // ITK_MANUAL_INSTANTIATION

#endif  // _itkCVTImageFilter_h
