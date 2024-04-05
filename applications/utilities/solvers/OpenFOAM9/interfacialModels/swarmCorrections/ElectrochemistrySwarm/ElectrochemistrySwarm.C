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

#include "ElectrochemistrySwarm.H"
#include "phasePair.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace swarmCorrections
{
    defineTypeNameAndDebug(ElectrochemistrySwarm, 0);
    addToRunTimeSelectionTable
    (
        swarmCorrection,
        ElectrochemistrySwarm,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::swarmCorrections::ElectrochemistrySwarm::ElectrochemistrySwarm
(
    const dictionary& dict,
    const phasePair& pair
)
:
    swarmCorrection(dict, pair),
    residualAlpha_
    (
        "residualAlpha",
        dimless,
        dict.lookupOrDefault<scalar>
        (
            "residualAlpha",
            pair_.dispersed().residualAlpha().value()
        )
    ),
    l_("l", dimless, dict),
    //residualU_("residualU", dimVelocity, dict),
    u0_("u0", dimless, dict), //dimVelocity
    ul0_("ul0", dimless, dict)//dimVelocity
    
    //u0_("u0", dimVelocity, dict), 
    //ul0_("ul0", dimVelocity, dict)//
    
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::swarmCorrections::ElectrochemistrySwarm::~ElectrochemistrySwarm()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField>
Foam::swarmCorrections::ElectrochemistrySwarm::Cs() const
{
  
    return 
    max(this->pair_.continuous(), residualAlpha_)
    *pow((pow(max(this->pair_.continuous(), residualAlpha_), l_)
    +pow(u0_*max(this->pair_.dispersed(), residualAlpha_)/max(this->pair_.continuous(), residualAlpha_), l_)), -ul0_);
    
    
    //pow(u0_/max(this->pair_.continuous(), residualAlpha_),2.0)
 //   /pow(max(this->pair_.magUr(), ul0_),2.0) 
 //   *pow(max(this->pair_.continuous(), residualAlpha_), l_);
}


// ************************************************************************* //
