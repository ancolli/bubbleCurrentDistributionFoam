/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2014-2020 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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

#include "Frank1.H"
#include "phasePair.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace wallLubricationModels
{
    defineTypeNameAndDebug(Frank1, 0);
    addToRunTimeSelectionTable
    (
        wallLubricationModel,
        Frank1,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::wallLubricationModels::Frank1::Frank1
(
    const dictionary& dict,
    const phasePair& pair
)
:
    wallLubricationModel(dict, pair),
    Cwd_("Cwd", dimless, dict),
    Cwc_("Cwc", dimless, dict),
    p_(dict.lookup<scalar>("p"))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::wallLubricationModels::Frank1::~Frank1()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::volVectorField> Foam::wallLubricationModels::Frank1::Fi() const
{
    volVectorField Ur(pair_.Ur());

    const volVectorField& n(nWall());
    const volScalarField& y(yWall());

    volScalarField Eo(pair_.Eo());
    volScalarField yTilde(y/(Cwc_*pair_.dispersed().d()));

    return zeroGradWalls
    (
        (
            neg(Eo - 1)*0.47
          + pos0(Eo - 1)*neg(Eo - 5)*exp(-0.933*Eo + 0.179)
          + pos0(Eo - 5)*neg(Eo - 33)*(0.00599*Eo - 0.0187)
          + pos0(Eo - 33)*0.179
        )
       *max
        (
            dimensionedScalar(dimless/dimLength, 0),
            (1 - yTilde)/(Cwd_*y*pow(yTilde, p_ - 1))
        )
       *pair_.continuous().rho()
       *magSqr(Ur - (Ur & n)*n)
       *n
    );
}


// ************************************************************************* //
