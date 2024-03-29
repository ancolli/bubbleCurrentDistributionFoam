/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2018 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is NOT part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "potentialWallFlux.H"

#include "surfaceInterpolate.H"
#include "fvcSnGrad.H"
#include "wallPolyPatch.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
    defineTypeNameAndDebug(potentialWallFlux, 0);
    addToRunTimeSelectionTable(functionObject, potentialWallFlux, dictionary);
}
}

void Foam::functionObjects::potentialWallFlux::writeFileHeader(const label i)
{
    // Add headers to output data
    writeHeader(file(), "Wall flux");
    writeCommented(file(), "Time");
    writeTabbed(file(), "patch");
    writeTabbed(file(), "min current density (A/m2)");
    writeTabbed(file(), "max current density (A/m2)");
    writeTabbed(file(), "current (A)");
    writeTabbed(file(), "Absolute current (A)");
    file() << endl;
}

void Foam::functionObjects::potentialWallFlux::calcFlux //void
(
    const volScalarField& keff_,
    const volScalarField& fi_, //const
    volScalarField& potentialWallFlux
)

{
    
    surfaceScalarField flux
    (
      fvc::interpolate(keff_)*fvc::snGrad(fi_)//
    );

    volScalarField::Boundary& potentialWallFluxBf =
        potentialWallFlux.boundaryFieldRef();

    const surfaceScalarField::Boundary& fluxBf =
        flux.boundaryField();

    forAll(potentialWallFluxBf, patchi)
    {
        potentialWallFluxBf[patchi] = fluxBf[patchi];
    }
}
// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


Foam::functionObjects::potentialWallFlux::potentialWallFlux
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    fvMeshFunctionObject(name, runTime, dict),

    logFiles(obr_, name),
    writeLocalObjects(obr_, log),
    patchSet_(),
    fvOptions_(mesh_),

    keff_
    (
       IOobject
       (
           "keff",//
           //"keffMean",
           mesh_.time().timeName(),
           mesh_,
           IOobject::MUST_READ,
           IOobject::NO_WRITE
       ),
       mesh_
    ),
 
    fi_
    (
       IOobject
       (
          "fi",//
          //"fiMean",
           mesh_.time().timeName(),
           mesh_,
           IOobject::MUST_READ,
           IOobject::NO_WRITE
       ),
       mesh_
      
    )
    

    
{
    volScalarField* potentialWallFluxPtr
    (
	new volScalarField
        (
            IOobject
            (
                type(),
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh_,
            dimensionedScalar("0", fi_.dimensions()*keff_.dimensions()/dimLength, 0.0)
        )
    );


    
    mesh_.objectRegistry::store(potentialWallFluxPtr);
    
    read(dict);
    resetName(typeName);
    resetLocalObjectName(typeName);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::functionObjects::potentialWallFlux::~potentialWallFlux()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::functionObjects::potentialWallFlux::read(const dictionary& dict)
{


    fvMeshFunctionObject::read(dict);
    writeLocalObjects::read(dict);
     
    const polyBoundaryMesh& pbm = mesh_.boundaryMesh();

    patchSet_ =
        mesh_.boundaryMesh().patchSet
        (
            wordReList(dict.lookupOrDefault("patches", wordReList()))
        );

    Info<< type() << " " << name() << ":" << nl;

    if (patchSet_.empty())
    {
        forAll(pbm, patchi)
        {
            if (isA<wallPolyPatch>(pbm[patchi]))
            {
                patchSet_.insert(patchi);
            }
        }

        Info<< "    processing all wall patches" << nl << endl;
    }
    else
    {
        Info<< "    processing wall patches: " << nl;
        labelHashSet filteredPatchSet;
        forAllConstIter(labelHashSet, patchSet_, iter)
        {
            label patchi = iter.key();
            if (isA<wallPolyPatch>(pbm[patchi]))
            {
                filteredPatchSet.insert(patchi);
                Info<< "        " << pbm[patchi].name() << endl;
            }
            else
            {
                WarningInFunction
                    << "Requested wall flux on non-wall boundary "
                    << "type patch: " << pbm[patchi].name() << endl;
            }
        }

        Info<< endl;

        patchSet_ = filteredPatchSet;
    }


    if (dict.found("fvOptions"))
    {
        fvOptions_.reset(dict.subDict("fvOptions"));
    }


    return true;
}


bool Foam::functionObjects::potentialWallFlux::execute()
{
    volScalarField& potentialWallFlux = lookupObjectRef<volScalarField>(type());
    
    volScalarField keff_
    (
       IOobject
       (
           "keff",//
           //"keffMean",
           mesh_.time().timeName(),
           mesh_,
           IOobject::MUST_READ,
           IOobject::NO_WRITE
       ),
       mesh_
    );
    
    volScalarField fi_
    (
       IOobject
       (
          "fi",//
          //"fiMean",//
           mesh_.time().timeName(),
           mesh_,
           IOobject::MUST_READ,
           IOobject::NO_WRITE
       ),
       mesh_
      
    );


    calcFlux(keff_, fi_, potentialWallFlux);

    return true;
}


bool Foam::functionObjects::potentialWallFlux::end()
{

    return true;
}


bool Foam::functionObjects::potentialWallFlux::write()
{
    Log << type() << " " << name() << " write:" << nl;

    writeLocalObjects::write();

    logFiles::write();

    const volScalarField& potentialWallFlux =
        obr_.lookupObject<volScalarField>(type());

    const fvPatchList& patches = mesh_.boundary();

    const surfaceScalarField::Boundary& magSf =
        mesh_.magSf().boundaryField();

    forAllConstIter(labelHashSet, patchSet_, iter)
    {
        label patchi = iter.key();
        const fvPatch& pp = patches[patchi];

        const scalarField& hfp = potentialWallFlux.boundaryField()[patchi];

        const scalar minHfp = gMin(hfp);
        const scalar maxHfp = gMax(hfp);
        const scalar integralHfp = gSum(magSf[patchi]*hfp);
	const scalar currentAbs = gSum(mag(magSf[patchi]*hfp)); //absolute value

        if (Pstream::master())
        {
            file()
                << mesh_.time().value()
                << tab << pp.name()
                << tab << minHfp
                << tab << maxHfp
                << tab << integralHfp
		<< tab << currentAbs
                << endl;
        }

        Log << "    min current density (A/m2)/max current density (A/m2)/current (A)/ Absolute current (A) (" << pp.name() << ") = "
            << minHfp << ", " << maxHfp << ", " << integralHfp << ", " << currentAbs << endl;
    }

    Log << endl;

    return true;
}


// ************************************************************************* //
