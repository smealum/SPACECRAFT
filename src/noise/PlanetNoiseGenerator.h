#ifndef __PLANETGENERATOR_H__
#define __PLANETGENERATOR_H__

#include <noise/noise.h>
#include "noise/noiseutils.h"
#include <utils/glm.h>
#include "PlanetInfo.h"

// TODO tester d'enlever les cache ou de de les supprimer car ils ne conservent que la dernière valeur demandée

class PlanetNoiseGenerator {
	const PlanetInfo &pi;
	// noise modules
    // 1: [Continent module]: This Perlin-noise module generates the continents.
    //    This noise module has a high number of octaves so that detail is
    //    visible at high zoom levels.
    noise::module::Perlin baseContinentDef_pe0;
    // 2: [Continent-with-ranges module]: Next, a curve module modifies the
    //    output value from the continent module so that very high values appear
    //    near sea level.  This defines the positions of the mountain ranges.
    noise::module::Curve baseContinentDef_cu;
    // 3: [Carver module]: This higher-frequency Perlin-noise module will be
    //    used by subsequent noise modules to carve out chunks from the mountain
    //    ranges within the continent-with-ranges module so that the mountain
    //    ranges will not be complely impassible.
    noise::module::Perlin baseContinentDef_pe1;
    // 4: [Scaled-carver module]: This scale/bias module scales the output
    //    value from the carver module such that it is usually near 1.0.  This
    //    is required for step 5.
    noise::module::ScaleBias baseContinentDef_sb;
    // 5: [Carved-continent module]: This minimum-value module carves out chunks
    //    from the continent-with-ranges module.  It does this by ensuring that
    //    only the minimum of the output values from the scaled-carver module
    //    and the continent-with-ranges module contributes to the output value
    //    of this subgroup.  Most of the time, the minimum-value module will
    //    select the output value from the continents-with-ranges module since
    //    the output value from the scaled-carver module is usually near 1.0.
    //    Occasionally, the output value from the scaled-carver module will be
    //    less than the output value from the continent-with-ranges module, so
    //    in this case, the output value from the scaled-carver module is
    //    selected.
    noise::module::Min baseContinentDef_mi;
    // 6: [Clamped-continent module]: Finally, a clamp module modifies the
    //    carved-continent module to ensure that the output value of this
    //    subgroup is between -1.0 and 1.0.
    noise::module::Clamp baseContinentDef_cl;
    // 7: [Base-continent-definition subgroup]: Caches the output value from the
    //    clamped-continent module.
    noise::module::Cache baseContinentDef;
    noise::module::Turbulence continentDef_tu0;
    // 2: [Intermediate-turbulence module]: This turbulence module warps the
    //    output value from the coarse-turbulence module.  This turbulence has
    //    a higher frequency, but lower power, than the coarse-turbulence
    //    module, adding some intermediate detail to it.
    noise::module::Turbulence continentDef_tu1;
    // 3: [Warped-base-continent-definition module]: This turbulence module
    //    warps the output value from the intermediate-turbulence module.  This
    //    turbulence has a higher frequency, but lower power, than the
    //    intermediate-turbulence module, adding some fine detail to it.
    noise::module::Turbulence continentDef_tu2;
    // 4: [Select-turbulence module]: At this stage, the turbulence is applied
    //    to the entire base-continent-definition subgroup, producing some very
    //    rugged, unrealistic coastlines.  This selector module selects the
    //    output values from the (unwarped) base-continent-definition subgroup
    //    and the warped-base-continent-definition module, based on the output
    //    value from the (unwarped) base-continent-definition subgroup.  The
    //    selection boundary is near sea level and has a relatively smooth
    //    transition.  In effect, only the higher areas of the base-continent-
    //    definition subgroup become warped; the underwater and coastal areas
    //    remain unaffected.
    noise::module::Select continentDef_se;
    // 7: [Continent-definition group]: Caches the output value from the
    //    clamped-continent module.  This is the output value for the entire
    //    continent-definition group.
    noise::module::Cache continentDef;
    // 1: [Warped-continent module]: This turbulence module slightly warps the
    //    output value from the continent-definition group.  This prevents the
    //    rougher terrain from appearing exclusively at higher elevations.
    //    Rough areas may now appear in the the ocean, creating rocky islands
    //    and fjords.
    noise::module::Turbulence terrainTypeDef_tu;
    // 2: [Roughness-probability-shift module]: This terracing module sharpens
    //    the edges of the warped-continent module near sea level and lowers
    //    the slope towards the higher-elevation areas.  This shrinks the areas
    //    in which the rough terrain appears, increasing the "rarity" of rough
    //    terrain.
    noise::module::Terrace terrainTypeDef_te;
    // 3: [Terrain-type-definition group]: Caches the output value from the
    //    roughness-probability-shift module.  This is the output value for
    //    the entire terrain-type-definition group.
    noise::module::Cache terrainTypeDef;
    // 1: [Mountain-ridge module]: This ridged-multifractal-noise module
    //    generates the mountain ridges.
    noise::module::RidgedMulti mountainBaseDef_rm0;
    // 2: [Scaled-mountain-ridge module]: Next, a scale/bias module scales the
    //    output value from the mountain-ridge module so that its ridges are not
    //    too high.  The reason for this is that another subgroup adds actual
    //    mountainous terrain to these ridges.
    noise::module::ScaleBias mountainBaseDef_sb0;
    // 3: [River-valley module]: This ridged-multifractal-noise module generates
    //    the river valleys.  It has a much lower frequency than the mountain-
    //    ridge module so that more mountain ridges will appear outside of the
    //    valleys.  Note that this noise module generates ridged-multifractal
    //    noise using only one octave; this information will be important in the
    //    next step.
    noise::module::RidgedMulti mountainBaseDef_rm1;
    // 4: [Scaled-river-valley module]: Next, a scale/bias module applies a
    //    scaling factor of -2.0 to the output value from the river-valley
    //    module.  This stretches the possible elevation values because one-
    //    octave ridged-multifractal noise has a lower range of output values
    //    than multiple-octave ridged-multifractal noise.  The negative scaling
    //    factor inverts the range of the output value, turning the ridges from
    //    the river-valley module into valleys.
    noise::module::ScaleBias mountainBaseDef_sb1;
    // 5: [Low-flat module]: This low constant value is used by step 6.
    noise::module::Const mountainBaseDef_co;
    // 6: [Mountains-and-valleys module]: This blender module merges the
    //    scaled-mountain-ridge module and the scaled-river-valley module
    //    together.  It causes the low-lying areas of the terrain to become
    //    smooth, and causes the high-lying areas of the terrain to contain
    //    ridges.  To do this, it uses the scaled-river-valley module as the
    //    control module, causing the low-flat module to appear in the lower
    //    areas and causing the scaled-mountain-ridge module to appear in the
    //    higher areas.
    noise::module::Blend mountainBaseDef_bl;
    // 7: [Coarse-turbulence module]: This turbulence module warps the output
    //    value from the mountain-and-valleys module, adding some coarse detail
    //    to it.
    noise::module::Turbulence mountainBaseDef_tu0;
    // 8: [Warped-mountains-and-valleys module]: This turbulence module warps
    //    the output value from the coarse-turbulence module.  This turbulence
    //    has a higher frequency, but lower power, than the coarse-turbulence
    //    module, adding some fine detail to it.
    noise::module::Turbulence mountainBaseDef_tu1;
    // 9: [Mountain-base-definition subgroup]: Caches the output value from the
    //    warped-mountains-and-valleys module.
    noise::module::Cache mountainBaseDef;
    // 1: [Mountain-basis-0 module]: This ridged-multifractal-noise module,
    //    along with the mountain-basis-1 module, generates the individual
    //    mountains.
    noise::module::RidgedMulti mountainousHigh_rm0;
    // 2: [Mountain-basis-1 module]: This ridged-multifractal-noise module,
    //    along with the mountain-basis-0 module, generates the individual
    //    mountains.
    noise::module::RidgedMulti mountainousHigh_rm1;
    // 3: [High-mountains module]: Next, a maximum-value module causes more
    //    mountains to appear at the expense of valleys.  It does this by
    //    ensuring that only the maximum of the output values from the two
    //    ridged-multifractal-noise modules contribute to the output value of
    //    this subgroup.
    noise::module::Max mountainousHigh_ma;
    // 4: [Warped-high-mountains module]: This turbulence module warps the
    //    output value from the high-mountains module, adding some detail to it.
    noise::module::Turbulence mountainousHigh_tu;
    // 5: [High-mountainous-terrain subgroup]: Caches the output value from the
    //    warped-high-mountains module.
    noise::module::Cache mountainousHigh;
    // 1: [Lowland-basis-0 module]: This ridged-multifractal-noise module,
    //    along with the lowland-basis-1 module, produces the low mountainous
    //    terrain.
    noise::module::RidgedMulti mountainousLow_rm0;
    // 1: [Lowland-basis-1 module]: This ridged-multifractal-noise module,
    //    along with the lowland-basis-0 module, produces the low mountainous
    //    terrain.
    noise::module::RidgedMulti mountainousLow_rm1;
    // 3: [Low-mountainous-terrain module]: This multiplication module combines
    //    the output values from the two ridged-multifractal-noise modules.
    //    This causes the following to appear in the resulting terrain:
    //    - Cracks appear when two negative output values are multiplied
    //      together.
    //    - Flat areas appear when a positive and a negative output value are
    //      multiplied together.
    //    - Ridges appear when two positive output values are multiplied
    //      together.
    noise::module::Multiply mountainousLow_mu;
    // 4: [Low-mountainous-terrain subgroup]: Caches the output value from the
    //    low-moutainous-terrain module.
    noise::module::Cache mountainousLow;
    // 1: [Scaled-low-mountainous-terrain module]: First, this scale/bias module
    //    scales the output value from the low-mountainous-terrain subgroup to a
    //    very low value and biases it towards -1.0.  This results in the low
    //    mountainous areas becoming more-or-less flat with little variation.
    //    This will also result in the low mountainous areas appearing at the
    //    lowest elevations in this subgroup.
    noise::module::ScaleBias mountainousTerrain_sb0;
    // 2: [Scaled-high-mountainous-terrain module]: Next, this scale/bias module
    //    scales the output value from the high-mountainous-terrain subgroup to
    //    1/4 of its initial value and biases it so that its output value is
    //    usually positive.
    noise::module::ScaleBias mountainousTerrain_sb1;
    // 3: [Added-high-mountainous-terrain module]: This addition module adds the
    //    output value from the scaled-high-mountainous-terrain module to the
    //    output value from the mountain-base-definition subgroup.  Mountains
    //    now appear all over the terrain.
    noise::module::Add mountainousTerrain_ad;
    // 4: [Combined-mountainous-terrain module]: Note that at this point, the
    //    entire terrain is covered in high mountainous terrain, even at the low
    //    elevations.  To make sure the mountains only appear at the higher
    //    elevations, this selector module causes low mountainous terrain to
    //    appear at the low elevations (within the valleys) and the high
    //    mountainous terrain to appear at the high elevations (within the
    //    ridges.)  To do this, this noise module selects the output value from
    //    the added-high-mountainous-terrain module if the output value from the
    //    mountain-base-definition subgroup is higher than a set amount.
    //    Otherwise, this noise module selects the output value from the scaled-
    //    low-mountainous-terrain module.
    noise::module::Select mountainousTerrain_se;
    // 5: [Scaled-mountainous-terrain-module]: This scale/bias module slightly
    //    reduces the range of the output value from the combined-mountainous-
    //    terrain module, decreasing the heights of the mountain peaks.
    noise::module::ScaleBias mountainousTerrain_sb2;
    // 6: [Glaciated-mountainous-terrain-module]: This exponential-curve module
    //    applies an exponential curve to the output value from the scaled-
    //    mountainous-terrain module.  This causes the slope of the mountains to
    //    smoothly increase towards higher elevations, as if a glacier grinded
    //    out those mountains.  This exponential-curve module expects the output
    //    value to range from -1.0 to +1.0.
    noise::module::Exponent mountainousTerrain_ex;
    // 7: [Mountainous-terrain group]: Caches the output value from the
    //    glaciated-mountainous-terrain module.  This is the output value for
    //    the entire mountainous-terrain group.
    noise::module::Cache mountainousTerrain;
    // 1: [Hills module]: This billow-noise module generates the hills.
    noise::module::Billow hillyTerrain_bi;
    // 2: [Scaled-hills module]: Next, a scale/bias module scales the output
    //    value from the hills module so that its hilltops are not too high.
    //    The reason for this is that these hills are eventually added to the
    //    river valleys (see below.)
    noise::module::ScaleBias hillyTerrain_sb0;
    // 3: [River-valley module]: This ridged-multifractal-noise module generates
    //    the river valleys.  It has a much lower frequency so that more hills
    //    will appear in between the valleys.  Note that this noise module
    //    generates ridged-multifractal noise using only one octave; this
    //    information will be important in the next step.
    noise::module::RidgedMulti hillyTerrain_rm;
    // 4: [Scaled-river-valley module]: Next, a scale/bias module applies a
    //    scaling factor of -2.0 to the output value from the river-valley
    //    module.  This stretches the possible elevation values because one-
    //    octave ridged-multifractal noise has a lower range of output values
    //    than multiple-octave ridged-multifractal noise.  The negative scaling
    //    factor inverts the range of the output value, turning the ridges from
    //    the river-valley module into valleys.
    noise::module::ScaleBias hillyTerrain_sb1;
    // 5: [Low-flat module]: This low constant value is used by step 6.
    noise::module::Const hillyTerrain_co;
    // 6: [Mountains-and-valleys module]: This blender module merges the
    //    scaled-hills module and the scaled-river-valley module together.  It
    //    causes the low-lying areas of the terrain to become smooth, and causes
    //    the high-lying areas of the terrain to contain hills.  To do this, it
    //    uses the scaled-hills module as the control module, causing the low-
    //    flat module to appear in the lower areas and causing the scaled-river-
    //    valley module to appear in the higher areas.
    noise::module::Blend hillyTerrain_bl;
    // 7: [Scaled-hills-and-valleys module]: This scale/bias module slightly
    //    reduces the range of the output value from the hills-and-valleys
    //    module, decreasing the heights of the hilltops.
    noise::module::ScaleBias hillyTerrain_sb2;
    // 8: [Increased-slope-hilly-terrain module]: To increase the hill slopes at
    //    higher elevations, this exponential-curve module applies an
    //    exponential curve to the output value the scaled-hills-and-valleys
    //    module.  This exponential-curve module expects the input value to
    //    range from -1.0 to 1.0.
    noise::module::Exponent hillyTerrain_ex;
    // 9: [Coarse-turbulence module]: This turbulence module warps the output
    //    value from the increased-slope-hilly-terrain module, adding some
    //    coarse detail to it.
    noise::module::Turbulence hillyTerrain_tu0;
    // 10: [Warped-hilly-terrain module]: This turbulence module warps the
    //     output value from the coarse-turbulence module.  This turbulence has
    //     a higher frequency, but lower power, than the coarse-turbulence
    //     module, adding some fine detail to it.
    noise::module::Turbulence hillyTerrain_tu1;
    // 11: [Hilly-terrain group]: Caches the output value from the warped-hilly-
    //     terrain module.  This is the output value for the entire hilly-
    //     terrain group.
    noise::module::Cache hillyTerrain;
    // 1: [Plains-basis-0 module]: This billow-noise module, along with the
    //    plains-basis-1 module, produces the plains.
    noise::module::Billow plainsTerrain_bi0;
    // 2: [Positive-plains-basis-0 module]: This scale/bias module makes the
    //    output value from the plains-basis-0 module positive since this output
    //    value will be multiplied together with the positive-plains-basis-1
    //    module.
    noise::module::ScaleBias plainsTerrain_sb0;
    // 3: [Plains-basis-1 module]: This billow-noise module, along with the
    //    plains-basis-2 module, produces the plains.
    noise::module::Billow plainsTerrain_bi1;
    // 4: [Positive-plains-basis-1 module]: This scale/bias module makes the
    //    output value from the plains-basis-1 module positive since this output
    //    value will be multiplied together with the positive-plains-basis-0
    //    module.
    noise::module::ScaleBias plainsTerrain_sb1;
    // 5: [Combined-plains-basis module]: This multiplication module combines
    //    the two plains basis modules together.
    noise::module::Multiply plainsTerrain_mu;
    // 6: [Rescaled-plains-basis module]: This scale/bias module maps the output
    //    value that ranges from 0.0 to 1.0 back to a value that ranges from
    //    -1.0 to +1.0.
    noise::module::ScaleBias plainsTerrain_sb2;
    // 7: [Plains-terrain group]: Caches the output value from the rescaled-
    //    plains-basis module.  This is the output value for the entire plains-
    //    terrain group.
    noise::module::Cache plainsTerrain;
    // 1: [Sand-dunes module]: This ridged-multifractal-noise module generates
    //    sand dunes.  This ridged-multifractal noise is generated with a single
    //    octave, which makes very smooth dunes.
    noise::module::RidgedMulti badlandsSand_rm;
    // 2: [Scaled-sand-dunes module]: This scale/bias module shrinks the dune
    //    heights by a small amount.  This is necessary so that the subsequent
    //    noise modules in this subgroup can add some detail to the dunes.
    noise::module::ScaleBias badlandsSand_sb0;
    // 3: [Dune-detail module]: This noise module uses Voronoi polygons to
    //    generate the detail to add to the dunes.  By enabling the distance
    //    algorithm, small polygonal pits are generated; the edges of the pits
    //    are joined to the edges of nearby pits.
    noise::module::Voronoi badlandsSand_vo;
    // 4: [Scaled-dune-detail module]: This scale/bias module shrinks the dune
    //    details by a large amount.  This is necessary so that the subsequent
    //    noise modules in this subgroup can add this detail to the sand-dunes
    //    module.
    noise::module::ScaleBias badlandsSand_sb1;
    // 5: [Dunes-with-detail module]: This addition module combines the scaled-
    //    sand-dunes module with the scaled-dune-detail module.
    noise::module::Add badlandsSand_ad;
    // 6: [Badlands-sand subgroup]: Caches the output value from the dunes-with-
    //    detail module.
    noise::module::Cache badlandsSand;
    // 1: [Cliff-basis module]: This Perlin-noise module generates some coherent
    //    noise that will be used to generate the cliffs.
    noise::module::Perlin badlandsCliffs_pe;
    // 2: [Cliff-shaping module]: Next, this curve module applies a curve to the
    //    output value from the cliff-basis module.  This curve is initially
    //    very shallow, but then its slope increases sharply.  At the highest
    //    elevations, the curve becomes very flat again.  This produces the
    //    stereotypical Utah-style desert cliffs.
    noise::module::Curve badlandsCliffs_cu;
    // 3: [Clamped-cliffs module]: This clamping module makes the tops of the
    //    cliffs very flat by clamping the output value from the cliff-shaping
    //    module so that the tops of the cliffs are very flat.
    noise::module::Clamp badlandsCliffs_cl;
    // 4: [Terraced-cliffs module]: Next, this terracing module applies some
    //    terraces to the clamped-cliffs module in the lower elevations before
    //    the sharp cliff transition.
    noise::module::Terrace badlandsCliffs_te;
    // 5: [Coarse-turbulence module]: This turbulence module warps the output
    //    value from the terraced-cliffs module, adding some coarse detail to
    //    it.
    noise::module::Turbulence badlandsCliffs_tu0;
    // 6: [Warped-cliffs module]: This turbulence module warps the output value
    //    from the coarse-turbulence module.  This turbulence has a higher
    //    frequency, but lower power, than the coarse-turbulence module, adding
    //    some fine detail to it.
    noise::module::Turbulence badlandsCliffs_tu1;
    // 7: [Badlands-cliffs subgroup]: Caches the output value from the warped-
    //    cliffs module.
    noise::module::Cache badlandsCliffs;
    // 1: [Scaled-sand-dunes module]: This scale/bias module considerably
    //    flattens the output value from the badlands-sands subgroup and lowers
    //    this value to near -1.0.
    noise::module::ScaleBias badlandsTerrain_sb;
    // 2: [Dunes-and-cliffs module]: This maximum-value module causes the dunes
    //    to appear in the low areas and the cliffs to appear in the high areas.
    //    It does this by selecting the maximum of the output values from the
    //    scaled-sand-dunes module and the badlands-cliffs subgroup.
    noise::module::Max badlandsTerrain_ma;
    // 3: [Badlands-terrain group]: Caches the output value from the dunes-and-
    //    cliffs module.  This is the output value for the entire badlands-
    //    terrain group.
    noise::module::Cache badlandsTerrain;
    // 1: [Large-river-basis module]: This ridged-multifractal-noise module
    //    creates the large, deep rivers.
    noise::module::RidgedMulti riverPositions_rm0;
    // 2: [Large-river-curve module]: This curve module applies a curve to the
    //    output value from the large-river-basis module so that the ridges
    //    become inverted.  This creates the rivers.  This curve also compresses
    //    the edge of the rivers, producing a sharp transition from the land to
    //    the river bottom.
    noise::module::Curve riverPositions_cu0;
    /// 3: [Small-river-basis module]: This ridged-multifractal-noise module
    //     creates the small, shallow rivers.
    noise::module::RidgedMulti riverPositions_rm1;
    // 4: [Small-river-curve module]: This curve module applies a curve to the
    //    output value from the small-river-basis module so that the ridges
    //    become inverted.  This creates the rivers.  This curve also compresses
    //    the edge of the rivers, producing a sharp transition from the land to
    //    the river bottom.
    noise::module::Curve riverPositions_cu1;
    // 5: [Combined-rivers module]: This minimum-value module causes the small
    //    rivers to cut into the large rivers.  It does this by selecting the
    //    minimum output values from the large-river-curve module and the small-
    //    river-curve module.
    noise::module::Min riverPositions_mi;
    // 6: [Warped-rivers module]: This turbulence module warps the output value
    //    from the combined-rivers module, which twists the rivers.  The high
    //    roughness produces less-smooth rivers.
    noise::module::Turbulence riverPositions_tu;
    // 7: [River-positions group]: Caches the output value from the warped-
    //    rivers module.  This is the output value for the entire river-
    //    positions group.
    noise::module::Cache riverPositions;
    // 1: [Base-scaled-mountainous-terrain module]: This scale/bias module
    //    scales the output value from the mountainous-terrain group so that the
    //    output value is measured in planetary elevation units.
    noise::module::ScaleBias scaledMountainousTerrain_sb0;
    // 2: [Base-peak-modulation module]: At this stage, most mountain peaks have
    //    roughly the same elevation.  This Perlin-noise module generates some
    //    random values that will be used by subsequent noise modules to
    //    randomly change the elevations of the mountain peaks.
    noise::module::Perlin scaledMountainousTerrain_pe;
    // 3: [Peak-modulation module]: This exponential-curve module applies an
    //    exponential curve to the output value from the base-peak-modulation
    //    module.  This produces a small number of high values and a much larger
    //    number of low values.  This means there will be a few peaks with much
    //    higher elevations than the majority of the peaks, making the terrain
    //    features more varied.
    noise::module::Exponent scaledMountainousTerrain_ex;
    // 4: [Scaled-peak-modulation module]: This scale/bias module modifies the
    //    range of the output value from the peak-modulation module so that it
    //    can be used as the modulator for the peak-height-multiplier module.
    //    It is important that this output value is not much lower than 1.0.
    noise::module::ScaleBias scaledMountainousTerrain_sb1;
    // 5: [Peak-height-multiplier module]: This multiplier module modulates the
    //    heights of the mountain peaks from the base-scaled-mountainous-terrain
    //    module using the output value from the scaled-peak-modulation module.
    noise::module::Multiply scaledMountainousTerrain_mu;
    // 6: [Scaled-mountainous-terrain group]: Caches the output value from the
    //    peak-height-multiplier module.  This is the output value for the
    //    entire scaled-mountainous-terrain group.
    noise::module::Cache scaledMountainousTerrain;
    // 1: [Base-scaled-hilly-terrain module]: This scale/bias module scales the
    //    output value from the hilly-terrain group so that this output value is
    //    measured in planetary elevation units 
    noise::module::ScaleBias scaledHillyTerrain_sb0;
    // 2: [Base-hilltop-modulation module]: At this stage, most hilltops have
    //    roughly the same elevation.  This Perlin-noise module generates some
    //    random values that will be used by subsequent noise modules to
    //    randomly change the elevations of the hilltops.
    noise::module::Perlin scaledHillyTerrain_pe;
    // 3: [Hilltop-modulation module]: This exponential-curve module applies an
    //    exponential curve to the output value from the base-hilltop-modulation
    //    module.  This produces a small number of high values and a much larger
    //    number of low values.  This means there will be a few hilltops with
    //    much higher elevations than the majority of the hilltops, making the
    //    terrain features more varied.
    noise::module::Exponent scaledHillyTerrain_ex;
    // 4: [Scaled-hilltop-modulation module]: This scale/bias module modifies
    //    the range of the output value from the hilltop-modulation module so
    //    that it can be used as the modulator for the hilltop-height-multiplier
    //    module.  It is important that this output value is not much lower than
    //    1.0.
    noise::module::ScaleBias scaledHillyTerrain_sb1;
    // 5: [Hilltop-height-multiplier module]: This multiplier module modulates
    //    the heights of the hilltops from the base-scaled-hilly-terrain module
    //    using the output value from the scaled-hilltop-modulation module.
    noise::module::Multiply scaledHillyTerrain_mu;
    // 6: [Scaled-hilly-terrain group]: Caches the output value from the
    //    hilltop-height-multiplier module.  This is the output value for the
    //    entire scaled-hilly-terrain group.
    noise::module::Cache scaledHillyTerrain;
    // 1: [Scaled-plains-terrain module]: This scale/bias module greatly
    //    flattens the output value from the plains terrain.  This output value
    //    is measured in planetary elevation units 
    noise::module::ScaleBias scaledPlainsTerrain_sb;
    // 2: [Scaled-plains-terrain group]: Caches the output value from the
    //    scaled-plains-terrain module.  This is the output value for the entire
    //    scaled-plains-terrain group.
    noise::module::Cache scaledPlainsTerrain;
    // 1: [Scaled-badlands-terrain module]: This scale/bias module scales the
    //    output value from the badlands-terrain group so that it is measured
    //    in planetary elevation units 
    noise::module::ScaleBias scaledBadlandsTerrain_sb;
    // 2: [Scaled-badlands-terrain group]: Caches the output value from the
    //    scaled-badlands-terrain module.  This is the output value for the
    //    entire scaled-badlands-terrain group.
    noise::module::Cache scaledBadlandsTerrain;
    // 1: [Shelf-creator module]: This terracing module applies a terracing
    //    curve to the continent-definition group at the specified shelf level.
    //    This terrace becomes the continental shelf.  Note that this terracing
    //    module also places another terrace below the continental shelf near
    //    -1.0.  The bottom of this terrace is defined as the bottom of the
    //    ocean; subsequent noise modules will later add oceanic trenches to the
    //    bottom of the ocean.
    noise::module::Terrace continentalShelf_te;
    // 2: [Oceanic-trench-basis module]: This ridged-multifractal-noise module
    //    generates some coherent noise that will be used to generate the
    //    oceanic trenches.  The ridges represent the bottom of the trenches.
    noise::module::RidgedMulti continentalShelf_rm;
    // 3: [Oceanic-trench module]: This scale/bias module inverts the ridges
    //    from the oceanic-trench-basis-module so that the ridges become
    //    trenches.  This noise module also reduces the depth of the trenches so
    //    that their depths are measured in planetary elevation units.
    noise::module::ScaleBias continentalShelf_sb;
    // 4: [Clamped-sea-bottom module]: This clamping module clamps the output
    //    value from the shelf-creator module so that its possible range is
    //    from the bottom of the ocean to sea level.  This is done because this
    //    subgroup is only concerned about the oceans.
    noise::module::Clamp continentalShelf_cl;
    // 5: [Shelf-and-trenches module]: This addition module adds the oceanic
    //    trenches to the clamped-sea-bottom module.
    noise::module::Add continentalShelf_ad;
    // 6: [Continental-shelf subgroup]: Caches the output value from the shelf-
    //    and-trenches module.
    noise::module::Cache continentalShelf;
    // 1: [Base-scaled-continent-elevations module]: This scale/bias module
    //    scales the output value from the continent-definition group so that it
    //    is measured in planetary elevation units 
    noise::module::ScaleBias baseContinentElev_sb;
    // 2: [Base-continent-with-oceans module]: This selector module applies the
    //    elevations of the continental shelves to the base elevations of the
    //    continent.  It does this by selecting the output value from the
    //    continental-shelf subgroup if the corresponding output value from the
    //    continent-definition group is below the shelf level.  Otherwise, it
    //    selects the output value from the base-scaled-continent-elevations
    //    module.
    noise::module::Select baseContinentElev_se;
    // 3: [Base-continent-elevation subgroup]: Caches the output value from the
    //    base-continent-with-oceans module.
    noise::module::Cache baseContinentElev;
    // 1: [Continents-with-plains module]:  This addition module adds the
    //    scaled-plains-terrain group to the base-continent-elevation subgroup.
    noise::module::Add continentsWithPlains_ad;
    // 2: [Continents-with-plains subgroup]: Caches the output value from the
    //    continents-with-plains module.
    noise::module::Cache continentsWithPlains;
    // 1: [Continents-with-hills module]:  This addition module adds the scaled-
    //    hilly-terrain group to the base-continent-elevation subgroup.
    noise::module::Add continentsWithHills_ad;
    // 2: [Select-high-elevations module]: This selector module ensures that
    //    the hills only appear at higher elevations.  It does this by selecting
    //    the output value from the continent-with-hills module if the
    //    corresponding output value from the terrain-type-defintion group is
    //    above a certain value. Otherwise, it selects the output value from the
    //    continents-with-plains subgroup.
    noise::module::Select continentsWithHills_se;
    // 3: [Continents-with-hills subgroup]: Caches the output value from the
    //    select-high-elevations module.
    noise::module::Cache continentsWithHills;
    // 1: [Continents-and-mountains module]:  This addition module adds the
    //    scaled-mountainous-terrain group to the base-continent-elevation
    //    subgroup.
    noise::module::Add continentsWithMountains_ad0;
    // 2: [Increase-mountain-heights module]:  This curve module applies a curve
    //    to the output value from the continent-definition group.  This
    //    modified output value is used by a subsequent noise module to add
    //    additional height to the mountains based on the current continent
    //    elevation.  The higher the continent elevation, the higher the
    //    mountains.
    noise::module::Curve continentsWithMountains_cu;
    // 3: [Add-increased-mountain-heights module]: This addition module adds
    //    the increased-mountain-heights module to the continents-and-
    //    mountains module.  The highest continent elevations now have the
    //    highest mountains.
    noise::module::Add continentsWithMountains_ad1;
    // 4: [Select-high-elevations module]: This selector module ensures that
    //    mountains only appear at higher elevations.  It does this by selecting
    //    the output value from the continent-with-mountains module if the
    //    corresponding output value from the terrain-type-defintion group is
    //    above a certain value.  Otherwise, it selects the output value from
    //    the continents-with-hills subgroup.  Note that the continents-with-
    //    hills subgroup also contains the plains terrain.
    noise::module::Select continentsWithMountains_se;
    // 5: [Continents-with-mountains subgroup]: Caches the output value from
    //    the select-high-elevations module.
    noise::module::Cache continentsWithMountains;
    // 1: [Badlands-positions module]: This Perlin-noise module generates some
    //    random noise, which is used by subsequent noise modules to specify the
    //    locations of the badlands.
    noise::module::Perlin continentsWithBadlands_pe;
    // 2: [Continents-and-badlands module]:  This addition module adds the
    //    scaled-badlands-terrain group to the base-continent-elevation
    //    subgroup.
    noise::module::Add continentsWithBadlands_ad;
    // 3: [Select-badlands-positions module]: This selector module places
    //    badlands at random spots on the continents based on the Perlin noise
    //    generated by the badlands-positions module.  To do this, it selects
    //    the output value from the continents-and-badlands module if the
    //    corresponding output value from the badlands-position module is
    //    greater than a specified value.  Otherwise, this selector module
    //    selects the output value from the continents-with-mountains subgroup.
    //    There is also a wide transition between these two noise modules so
    //    that the badlands can blend into the rest of the terrain on the
    //    continents.
    noise::module::Select continentsWithBadlands_se;
    // 4: [Apply-badlands module]: This maximum-value module causes the badlands
    //    to "poke out" from the rest of the terrain.  It does this by ensuring
    //    that only the maximum of the output values from the continents-with-
    //    mountains subgroup and the select-badlands-positions modules
    //    contribute to the output value of this subgroup.  One side effect of
    //    this process is that the badlands will not appear in mountainous
    //    terrain.
    noise::module::Max continentsWithBadlands_ma;
    // 5: [Continents-with-badlands subgroup]: Caches the output value from the
    //    apply-badlands module.
    noise::module::Cache continentsWithBadlands;
    // 1: [Scaled-rivers module]: This scale/bias module scales the output value
    //    from the river-positions group so that it is measured in planetary
    //    elevation units and is negative; this is required for step 2.
    noise::module::ScaleBias continentsWithRivers_sb;
    // 2: [Add-rivers-to-continents module]: This addition module adds the
    //    rivers to the continents-with-badlands subgroup.  Because the scaled-
    //    rivers module only outputs a negative value, the scaled-rivers module
    //    carves the rivers out of the terrain.
    noise::module::Add continentsWithRivers_ad;
    // 3: [Blended-rivers-to-continents module]: This selector module outputs
    //    deep rivers near sea level and shallower rivers in higher terrain.  It
    //    does this by selecting the output value from the continents-with-
    //    badlands subgroup if the corresponding output value from the
    //    continents-with-badlands subgroup is far from sea level.  Otherwise,
    //    this selector module selects the output value from the add-rivers-to-
    //    continents module.
    noise::module::Select continentsWithRivers_se;
    // 4: [Continents-with-rivers subgroup]: Caches the output value from the
    //    blended-rivers-to-continents module.
    // XXX ce cache c'est de la merde, a refaire pour nos besoins
    noise::module::Cache continentsWithRivers;

	public:
	PlanetNoiseGenerator(const PlanetInfo& pinfo);
	~PlanetNoiseGenerator();

	inline float getElevation(const glm::vec3 &v)
	{
		return continentsWithRivers.GetValue(v.x, v.y, v.z);
	}

};

#endif
