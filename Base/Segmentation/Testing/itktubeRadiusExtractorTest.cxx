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

#include "itktubeRadiusExtractor.h"
#include "tubeMessage.h"

#include <itkImageFileReader.h>
#include <itkMersenneTwisterRandomVariateGenerator.h>
#include <itkSpatialObjectReader.h>

int itktubeRadiusExtractorTest( int argc, char * argv[] )
  {
  if( argc != 3 )
    {
    std::cout
      << "itktubeRadiusExtractorTest <inputImage> <vessel.tre>"
      << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image<float, 3>   ImageType;

  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  ImageReaderType::Pointer imReader = ImageReaderType::New();
  imReader->SetFileName( argv[1] );
  imReader->Update();

  ImageType::Pointer im = imReader->GetOutput();

  typedef itk::tube::RadiusExtractor<ImageType> RadiusOpType;
  RadiusOpType::Pointer radiusOp = RadiusOpType::New();

  radiusOp->SetInputImage( im );

  bool returnStatus = EXIT_SUCCESS;

  double dataMin = radiusOp->GetDataMin();
  if( dataMin != 54 )
    {
    tube::ErrorMessage( "Data min != 54" );
    returnStatus = EXIT_FAILURE;
    }

  double dataMax = radiusOp->GetDataMax();
  if( dataMax != 230 )
    {
    tube::ErrorMessage( "Data max != 230" );
    returnStatus = EXIT_FAILURE;
    }

  radiusOp->SetRadiusMin( 0.75 );
  if( radiusOp->GetRadiusMin() != 0.75 )
    {
    tube::ErrorMessage( "Radius min != 0.75" );
    returnStatus = EXIT_FAILURE;
    }

  radiusOp->SetRadiusMax( 10.0 );
  if( radiusOp->GetRadiusMax() != 10.0 )
    {
    tube::ErrorMessage( "Radius max != 10.0" );
    returnStatus = EXIT_FAILURE;
    }

  radiusOp->SetRadiusStart( 2.0 );
  if( radiusOp->GetRadiusStart() != 2.0 )
    {
    tube::ErrorMessage( "Radius 0 != 2.0" );
    returnStatus = EXIT_FAILURE;
    }

  radiusOp->SetMinMedialness( 0.005 );
  if( radiusOp->GetMinMedialness() != 0.005 )
    {
    tube::ErrorMessage( "MinMedialness != 0.005" );
    returnStatus = EXIT_FAILURE;
    }

  radiusOp->SetMinMedialnessStart( 0.002 );
  if( radiusOp->GetMinMedialnessStart() != 0.002 )
    {
    tube::ErrorMessage( "MinMedialnessStart != 0.002" );
    returnStatus = EXIT_FAILURE;
    }

  typedef itk::SpatialObjectReader<>                   ReaderType;
  typedef itk::SpatialObject<>::ChildrenListType       ObjectListType;
  typedef itk::GroupSpatialObject<>                    GroupType;
  typedef itk::VesselTubeSpatialObject<>               TubeType;
  typedef TubeType::PointListType                      PointListType;
  typedef TubeType::TubePointType                      TubePointType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();
  GroupType::Pointer group = reader->GetGroup();

  std::cout << "Number of children = " << group->GetNumberOfChildren()
    << std::endl;

  char tubeName[17];
  std::strcpy( tubeName, "Tube" );
  ObjectListType * tubeList = group->GetChildren( -1, tubeName );

  unsigned int numTubes = tubeList->size();
  std::cout << "Number of tubes = " << numTubes << std::endl;

  typedef itk::Statistics::MersenneTwisterRandomVariateGenerator
    RandGenType;
  RandGenType::Pointer rndGen = RandGenType::New();
  rndGen->Initialize(); // set seed here

  int failures = 0;
  unsigned int numMCRuns = 20;
  for( unsigned int mcRun=0; mcRun<numMCRuns; mcRun++ )
    {
    std::cout << std::endl;
    std::cout << "*** RUN = " << mcRun << std::endl;
    unsigned int rndTubeNum = rndGen->GetUniformVariate( 0, 1 ) * numTubes;
    if( rndTubeNum > numTubes-1 )
      {
      rndTubeNum = numTubes-1;
      }
    ObjectListType::iterator tubeIter = tubeList->begin();
    for( unsigned int i=0; i<rndTubeNum; i++ )
      {
      ++tubeIter;
      }
    TubeType * tubep = static_cast< TubeType * >(
      tubeIter->GetPointer() );
    TubeType::Pointer tube = static_cast< TubeType * >(
      tubeIter->GetPointer() );
    tube::ComputeTubeTangentsAndNormals< TubeType >( tubep );

    std::cout << "Test tube = " << rndTubeNum << std::endl;

    PointListType tubePointList = tube->GetPoints();
    unsigned int numPoints = tubePointList.size();
    unsigned int rndPointNum = rndGen->GetUniformVariate( 0, 1 )
      * numPoints * 0.8 + numPoints * 0.1;
    if( rndPointNum > numPoints-1 )
      {
      rndPointNum = numPoints-1;
      }
    PointListType::iterator pntIter = tubePointList.begin();
    for( unsigned int i=0; i<rndPointNum; i++ )
      {
      ++pntIter;
      }
    TubePointType * pnt = static_cast< TubePointType * >(&(*pntIter));
    std::cout << "Test point = " << rndPointNum << std::endl;

    double r0 = pnt->GetRadius();
    double r1 = r0;
    if( r1 < 1 )
      {
      r1 = 1;
      }

    std::cout << "  x = " << pnt->GetPosition() << std::endl;
    std::cout << "  r = " << r0 << std::endl;
    std::cout << "  t = " << pnt->GetTangent() << std::endl;
    std::cout << "  n1 = " << pnt->GetNormal1() << std::endl;
    std::cout << "  n2 = " << pnt->GetNormal2() << std::endl;

    double rMin = 0.75;
    double rMax = 10;
    double rStep = 0.25;
    double rTol = 0.1;
    if( !radiusOp->OptimalRadiusAtPoint( *pnt, r1, rMin, rMax,
      rStep, rTol ) )
      {
      std::cout << "OptimalRadius returned false." << std::endl;
      std::cout << "   Target = " << pnt->GetPosition()
        << " at r = " << r0 << std::endl;
      std::cout << "      Result r = " << r1 << std::endl;
      ++failures;
      continue;
      }

    double diff = vnl_math_abs( r1 - r0 );
    if( diff > 0.2 * r0 && diff > 0.75 )
      {
      std::cout << "Radius estimate inaccurate." << std::endl;
      std::cout << "   Target = " << r0 << std::endl;
      std::cout << "      Result = " << r1 << std::endl;
      ++failures;
      continue;
      }

    std::cout << "*** Radius estimate a success! ***" << std::endl;
    std::cout << "   Target = " << r0 << std::endl;
    std::cout << "      Result = " << r1 << std::endl;
    }

  delete tubeList;

  std::cout << "Number of failures = " << failures << std::endl;
  if( failures > 0.2 * numMCRuns )
    {
    return EXIT_FAILURE;
    }

  return returnStatus;
  }
