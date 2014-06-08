#include "noise/PlanetGenerator.h"

PlanetGenerator::PlanetGenerator(const PlanetInfo & pinfo):
	pi(pinfo)
{
    baseContinentDef_pe0.SetSeed (pi.seed + 0);
    baseContinentDef_pe0.SetFrequency (pi.continentFrequency);
    baseContinentDef_pe0.SetPersistence (0.5);
    baseContinentDef_pe0.SetLacunarity (pi.continentLacunarity);
    baseContinentDef_pe0.SetOctaveCount (14);
    baseContinentDef_pe0.SetNoiseQuality (QUALITY_STD);


    baseContinentDef_cu.SetSourceModule (0, baseContinentDef_pe0);
    baseContinentDef_cu.AddControlPoint (-2.0000 + pi.seaLevel,-1.625 + pi.seaLevel);
    baseContinentDef_cu.AddControlPoint (-1.0000 + pi.seaLevel,-1.375 + pi.seaLevel);
    baseContinentDef_cu.AddControlPoint ( 0.0000 + pi.seaLevel,-0.375 + pi.seaLevel);
    baseContinentDef_cu.AddControlPoint ( 0.0625 + pi.seaLevel, 0.125 + pi.seaLevel);
    baseContinentDef_cu.AddControlPoint ( 0.1250 + pi.seaLevel, 0.250 + pi.seaLevel);
    baseContinentDef_cu.AddControlPoint ( 0.2500 + pi.seaLevel, 1.000 + pi.seaLevel);
    baseContinentDef_cu.AddControlPoint ( 0.5000 + pi.seaLevel, 0.250 + pi.seaLevel);
    baseContinentDef_cu.AddControlPoint ( 0.7500 + pi.seaLevel, 0.250 + pi.seaLevel);
    baseContinentDef_cu.AddControlPoint ( 1.0000 + pi.seaLevel, 0.500 + pi.seaLevel);
    baseContinentDef_cu.AddControlPoint ( 2.0000 + pi.seaLevel, 0.500 + pi.seaLevel);


    baseContinentDef_pe1.SetSeed (pi.seed + 1);
    baseContinentDef_pe1.SetFrequency (pi.continentFrequency * 4.34375);
    baseContinentDef_pe1.SetPersistence (0.5);
    baseContinentDef_pe1.SetLacunarity (pi.continentLacunarity);
    baseContinentDef_pe1.SetOctaveCount (11);
    baseContinentDef_pe1.SetNoiseQuality (QUALITY_STD);


    baseContinentDef_sb.SetSourceModule (0, baseContinentDef_pe1);
    baseContinentDef_sb.SetScale (0.375);
    baseContinentDef_sb.SetBias (0.625);


    baseContinentDef_mi.SetSourceModule (0, baseContinentDef_sb);
    baseContinentDef_mi.SetSourceModule (1, baseContinentDef_cu);


    baseContinentDef_cl.SetSourceModule (0, baseContinentDef_mi);
    baseContinentDef_cl.SetBounds (-1.0, 1.0);


    baseContinentDef.SetSourceModule (0, baseContinentDef_cl);


    continentDef_tu0.SetSourceModule (0, baseContinentDef);
    continentDef_tu0.SetSeed (pi.seed + 10);
    continentDef_tu0.SetFrequency (pi.continentFrequency * 15.25);
    continentDef_tu0.SetPower (pi.continentFrequency / 113.75);
    continentDef_tu0.SetRoughness (13);


    continentDef_tu1.SetSourceModule (0, continentDef_tu0);
    continentDef_tu1.SetSeed (pi.seed + 11);
    continentDef_tu1.SetFrequency (pi.continentFrequency * 47.25);
    continentDef_tu1.SetPower (pi.continentFrequency / 433.75);
    continentDef_tu1.SetRoughness (12);


    continentDef_tu2.SetSourceModule (0, continentDef_tu1);
    continentDef_tu2.SetSeed (pi.seed + 12);
    continentDef_tu2.SetFrequency (pi.continentFrequency * 95.25);
    continentDef_tu2.SetPower (pi.continentFrequency / 1019.75);
    continentDef_tu2.SetRoughness (11);


    continentDef_se.SetSourceModule (0, baseContinentDef);
    continentDef_se.SetSourceModule (1, continentDef_tu2);
    continentDef_se.SetControlModule (baseContinentDef);
    continentDef_se.SetBounds (pi.seaLevel - 0.0375, pi.seaLevel + 1000.0375);
    continentDef_se.SetEdgeFalloff (0.0625);


    continentDef.SetSourceModule (0, continentDef_se);

    terrainTypeDef_tu.SetSourceModule (0, continentDef);
    terrainTypeDef_tu.SetSeed (pi.seed + 20);
    terrainTypeDef_tu.SetFrequency (pi.continentFrequency * 18.125);
    terrainTypeDef_tu.SetPower (pi.continentFrequency / 20.59375
            * pi.terrainOffset);
    terrainTypeDef_tu.SetRoughness (3);


    terrainTypeDef_te.SetSourceModule (0, terrainTypeDef_tu);
    terrainTypeDef_te.AddControlPoint (-1.00);
    terrainTypeDef_te.AddControlPoint (pi.shelfLevel + pi.seaLevel / 2.0);
    terrainTypeDef_te.AddControlPoint (1.00);


    terrainTypeDef.SetSourceModule (0, terrainTypeDef_te);

    mountainBaseDef_rm0.SetSeed (pi.seed + 30);
    mountainBaseDef_rm0.SetFrequency (1723.0);
    mountainBaseDef_rm0.SetLacunarity (pi.mountainLacunarity);
    mountainBaseDef_rm0.SetOctaveCount (4);
    mountainBaseDef_rm0.SetNoiseQuality (QUALITY_STD);


    mountainBaseDef_sb0.SetSourceModule (0, mountainBaseDef_rm0);
    mountainBaseDef_sb0.SetScale (0.5);
    mountainBaseDef_sb0.SetBias (0.375);

    mountainBaseDef_rm1.SetSeed (pi.seed + 31);
    mountainBaseDef_rm1.SetFrequency (367.0);
    mountainBaseDef_rm1.SetLacunarity (pi.mountainLacunarity);
    mountainBaseDef_rm1.SetOctaveCount (1);
    mountainBaseDef_rm1.SetNoiseQuality (QUALITY_BEST);

    mountainBaseDef_sb1.SetSourceModule (0, mountainBaseDef_rm1);
    mountainBaseDef_sb1.SetScale (-2.0);
    mountainBaseDef_sb1.SetBias (-0.5);

    mountainBaseDef_co.SetConstValue (-1.0);

    mountainBaseDef_bl.SetSourceModule (0, mountainBaseDef_co);
    mountainBaseDef_bl.SetSourceModule (1, mountainBaseDef_sb0);
    mountainBaseDef_bl.SetControlModule (mountainBaseDef_sb1);

    mountainBaseDef_tu0.SetSourceModule (0, mountainBaseDef_bl);
    mountainBaseDef_tu0.SetSeed (pi.seed + 32);
    mountainBaseDef_tu0.SetFrequency (1337.0);
    mountainBaseDef_tu0.SetPower (1.0 / 6730.0 * pi.mountainsTwist);
    mountainBaseDef_tu0.SetRoughness (4);

    mountainBaseDef_tu1.SetSourceModule (0, mountainBaseDef_tu0);
    mountainBaseDef_tu1.SetSeed (pi.seed + 33);
    mountainBaseDef_tu1.SetFrequency (21221.0);
    mountainBaseDef_tu1.SetPower (1.0 / 120157.0 * pi.mountainsTwist);
    mountainBaseDef_tu1.SetRoughness (6);

    mountainBaseDef.SetSourceModule (0, mountainBaseDef_tu1);
    mountainousHigh_rm0.SetSeed (pi.seed + 40);
    mountainousHigh_rm0.SetFrequency (2371.0);
    mountainousHigh_rm0.SetLacunarity (pi.mountainLacunarity);
    mountainousHigh_rm0.SetOctaveCount (3);
    mountainousHigh_rm0.SetNoiseQuality (QUALITY_BEST);

    mountainousHigh_rm1.SetSeed (pi.seed + 41);
    mountainousHigh_rm1.SetFrequency (2341.0);
    mountainousHigh_rm1.SetLacunarity (pi.mountainLacunarity);
    mountainousHigh_rm1.SetOctaveCount (3);
    mountainousHigh_rm1.SetNoiseQuality (QUALITY_BEST);

    mountainousHigh_ma.SetSourceModule (0, mountainousHigh_rm0);
    mountainousHigh_ma.SetSourceModule (1, mountainousHigh_rm1);

    mountainousHigh_tu.SetSourceModule (0, mountainousHigh_ma);
    mountainousHigh_tu.SetSeed (pi.seed + 42);
    mountainousHigh_tu.SetFrequency (31511.0);
    mountainousHigh_tu.SetPower (1.0 / 180371.0 * pi.mountainsTwist);
    mountainousHigh_tu.SetRoughness (4);

    mountainousHigh.SetSourceModule (0, mountainousHigh_tu);
    mountainousLow_rm0.SetSeed (pi.seed + 50);
    mountainousLow_rm0.SetFrequency (1381.0);
    mountainousLow_rm0.SetLacunarity (pi.mountainLacunarity);
    mountainousLow_rm0.SetOctaveCount (8);
    mountainousLow_rm0.SetNoiseQuality (QUALITY_BEST);

    mountainousLow_rm1.SetSeed (pi.seed + 51);
    mountainousLow_rm1.SetFrequency (1427.0);
    mountainousLow_rm1.SetLacunarity (pi.mountainLacunarity);
    mountainousLow_rm1.SetOctaveCount (8);
    mountainousLow_rm1.SetNoiseQuality (QUALITY_BEST);

    mountainousLow_mu.SetSourceModule (0, mountainousLow_rm0);
    mountainousLow_mu.SetSourceModule (1, mountainousLow_rm1);

    mountainousLow.SetSourceModule (0, mountainousLow_mu);
    mountainousTerrain_sb0.SetSourceModule (0, mountainousLow);
    mountainousTerrain_sb0.SetScale (0.03125);
    mountainousTerrain_sb0.SetBias (-0.96875);

    mountainousTerrain_sb1.SetSourceModule (0, mountainousHigh);
    mountainousTerrain_sb1.SetScale (0.25);
    mountainousTerrain_sb1.SetBias (0.25);

    mountainousTerrain_ad.SetSourceModule (0, mountainousTerrain_sb1);
    mountainousTerrain_ad.SetSourceModule (1, mountainBaseDef);

    mountainousTerrain_se.SetSourceModule (0, mountainousTerrain_sb0);
    mountainousTerrain_se.SetSourceModule (1, mountainousTerrain_ad);
    mountainousTerrain_se.SetControlModule (mountainBaseDef);
    mountainousTerrain_se.SetBounds (-0.5, 999.5);
    mountainousTerrain_se.SetEdgeFalloff (0.5);

    mountainousTerrain_sb2.SetSourceModule (0, mountainousTerrain_se);
    mountainousTerrain_sb2.SetScale (0.8);
    mountainousTerrain_sb2.SetBias (0.0);

    mountainousTerrain_ex.SetSourceModule (0, mountainousTerrain_sb2);
    mountainousTerrain_ex.SetExponent (pi.mountainGlaciation);

    mountainousTerrain.SetSourceModule (0, mountainousTerrain_ex);

    hillyTerrain_bi.SetSeed (pi.seed + 60);
    hillyTerrain_bi.SetFrequency (1663.0);
    hillyTerrain_bi.SetPersistence (0.5);
    hillyTerrain_bi.SetLacunarity (pi.hillsLacunarity);
    hillyTerrain_bi.SetOctaveCount (6);
    hillyTerrain_bi.SetNoiseQuality (QUALITY_BEST);

    hillyTerrain_sb0.SetSourceModule (0, hillyTerrain_bi);
    hillyTerrain_sb0.SetScale (0.5);
    hillyTerrain_sb0.SetBias (0.5);

    hillyTerrain_rm.SetSeed (pi.seed + 61);
    hillyTerrain_rm.SetFrequency (367.5);
    hillyTerrain_rm.SetLacunarity (pi.hillsLacunarity);
    hillyTerrain_rm.SetNoiseQuality (QUALITY_BEST);
    hillyTerrain_rm.SetOctaveCount (1);

    hillyTerrain_sb1.SetSourceModule (0, hillyTerrain_rm);
    hillyTerrain_sb1.SetScale (-2.0);
    hillyTerrain_sb1.SetBias (-0.5);

    hillyTerrain_co.SetConstValue (-1.0);

    hillyTerrain_bl.SetSourceModule (0, hillyTerrain_co);
    hillyTerrain_bl.SetSourceModule (1, hillyTerrain_sb1);
    hillyTerrain_bl.SetControlModule (hillyTerrain_sb0);

    hillyTerrain_sb2.SetSourceModule (0, hillyTerrain_bl);
    hillyTerrain_sb2.SetScale (0.75);
    hillyTerrain_sb2.SetBias (-0.25);

    hillyTerrain_ex.SetSourceModule (0, hillyTerrain_sb2);
    hillyTerrain_ex.SetExponent (1.375);

    hillyTerrain_tu0.SetSourceModule (0, hillyTerrain_ex);
    hillyTerrain_tu0.SetSeed (pi.seed + 62);
    hillyTerrain_tu0.SetFrequency (1531.0);
    hillyTerrain_tu0.SetPower (1.0 / 16921.0 * pi.hillsTwist);
    hillyTerrain_tu0.SetRoughness (4);

    hillyTerrain_tu1.SetSourceModule (0, hillyTerrain_tu0);
    hillyTerrain_tu1.SetSeed (pi.seed + 63);
    hillyTerrain_tu1.SetFrequency (21617.0);
    hillyTerrain_tu1.SetPower (1.0 / 117529.0 * pi.hillsTwist);
    hillyTerrain_tu1.SetRoughness (6);

    hillyTerrain.SetSourceModule (0, hillyTerrain_tu1);

    plainsTerrain_bi0.SetSeed (pi.seed + 70);
    plainsTerrain_bi0.SetFrequency (1097.5);
    plainsTerrain_bi0.SetPersistence (0.5);
    plainsTerrain_bi0.SetLacunarity (pi.plainsLacunarity);
    plainsTerrain_bi0.SetOctaveCount (8);
    plainsTerrain_bi0.SetNoiseQuality (QUALITY_BEST);

    plainsTerrain_sb0.SetSourceModule (0, plainsTerrain_bi0);
    plainsTerrain_sb0.SetScale (0.5);
    plainsTerrain_sb0.SetBias (0.5);

    plainsTerrain_bi1.SetSeed (pi.seed + 71);
    plainsTerrain_bi1.SetFrequency (1319.5);
    plainsTerrain_bi1.SetPersistence (0.5);
    plainsTerrain_bi1.SetLacunarity (pi.plainsLacunarity);
    plainsTerrain_bi1.SetOctaveCount (8);
    plainsTerrain_bi1.SetNoiseQuality (QUALITY_BEST);

    plainsTerrain_sb1.SetSourceModule (0, plainsTerrain_bi1);
    plainsTerrain_sb1.SetScale (0.5);
    plainsTerrain_sb1.SetBias (0.5);

    plainsTerrain_mu.SetSourceModule (0, plainsTerrain_sb0);
    plainsTerrain_mu.SetSourceModule (1, plainsTerrain_sb1);

    plainsTerrain_sb2.SetSourceModule (0, plainsTerrain_mu);
    plainsTerrain_sb2.SetScale (2.0);
    plainsTerrain_sb2.SetBias (-1.0);

    plainsTerrain.SetSourceModule (0, plainsTerrain_sb2);

    badlandsSand_rm.SetSeed (pi.seed + 80);
    badlandsSand_rm.SetFrequency (6163.5);
    badlandsSand_rm.SetLacunarity (pi.badlandsLacunarity);
    badlandsSand_rm.SetNoiseQuality (QUALITY_BEST);
    badlandsSand_rm.SetOctaveCount (1);

    badlandsSand_sb0.SetSourceModule (0, badlandsSand_rm);
    badlandsSand_sb0.SetScale (0.875);
    badlandsSand_sb0.SetBias (0.0);

    badlandsSand_vo.SetSeed (pi.seed + 81);
    badlandsSand_vo.SetFrequency (16183.25);
    badlandsSand_vo.SetDisplacement (0.0);
    badlandsSand_vo.EnableDistance ();

    badlandsSand_sb1.SetSourceModule (0, badlandsSand_vo);
    badlandsSand_sb1.SetScale (0.25);
    badlandsSand_sb1.SetBias (0.25);

    badlandsSand_ad.SetSourceModule (0, badlandsSand_sb0);
    badlandsSand_ad.SetSourceModule (1, badlandsSand_sb1);

    badlandsSand.SetSourceModule (0, badlandsSand_ad);

    badlandsCliffs_pe.SetSeed (pi.seed + 90);
    badlandsCliffs_pe.SetFrequency (pi.continentFrequency * 839.0);
    badlandsCliffs_pe.SetPersistence (0.5);
    badlandsCliffs_pe.SetLacunarity (pi.badlandsLacunarity);
    badlandsCliffs_pe.SetOctaveCount (6);
    badlandsCliffs_pe.SetNoiseQuality (QUALITY_STD);

    badlandsCliffs_cu.SetSourceModule (0, badlandsCliffs_pe);
    badlandsCliffs_cu.AddControlPoint (-2.0000, -2.0000);
    badlandsCliffs_cu.AddControlPoint (-1.0000, -1.2500);
    badlandsCliffs_cu.AddControlPoint (-0.0000, -0.7500);
    badlandsCliffs_cu.AddControlPoint ( 0.5000, -0.2500);
    badlandsCliffs_cu.AddControlPoint ( 0.6250,  0.8750);
    badlandsCliffs_cu.AddControlPoint ( 0.7500,  1.0000);
    badlandsCliffs_cu.AddControlPoint ( 2.0000,  1.2500);

    badlandsCliffs_cl.SetSourceModule (0, badlandsCliffs_cu);
    badlandsCliffs_cl.SetBounds (-999.125, 0.875);

    badlandsCliffs_te.SetSourceModule (0, badlandsCliffs_cl);
    badlandsCliffs_te.AddControlPoint (-1.0000);
    badlandsCliffs_te.AddControlPoint (-0.8750);
    badlandsCliffs_te.AddControlPoint (-0.7500);
    badlandsCliffs_te.AddControlPoint (-0.5000);
    badlandsCliffs_te.AddControlPoint ( 0.0000);
    badlandsCliffs_te.AddControlPoint ( 1.0000);

    badlandsCliffs_tu0.SetSeed (pi.seed + 91);
    badlandsCliffs_tu0.SetSourceModule (0, badlandsCliffs_te);
    badlandsCliffs_tu0.SetFrequency (16111.0);
    badlandsCliffs_tu0.SetPower (1.0 / 141539.0 * pi.badlandsTwist);
    badlandsCliffs_tu0.SetRoughness (3);

    badlandsCliffs_tu1.SetSeed (pi.seed + 92);
    badlandsCliffs_tu1.SetSourceModule (0, badlandsCliffs_tu0);
    badlandsCliffs_tu1.SetFrequency (36107.0);
    badlandsCliffs_tu1.SetPower (1.0 / 211543.0 * pi.badlandsTwist);
    badlandsCliffs_tu1.SetRoughness (3);

    badlandsCliffs.SetSourceModule (0, badlandsCliffs_tu1);

    badlandsTerrain_sb.SetSourceModule (0, badlandsSand);
    badlandsTerrain_sb.SetScale (0.25);
    badlandsTerrain_sb.SetBias (-0.75);

    badlandsTerrain_ma.SetSourceModule (0, badlandsCliffs);
    badlandsTerrain_ma.SetSourceModule (1, badlandsTerrain_sb);

    badlandsTerrain.SetSourceModule (0, badlandsTerrain_ma);

    riverPositions_rm0.SetSeed (pi.seed + 100);
    riverPositions_rm0.SetFrequency (18.75);
    riverPositions_rm0.SetLacunarity (pi.continentLacunarity);
    riverPositions_rm0.SetOctaveCount (1);
    riverPositions_rm0.SetNoiseQuality (QUALITY_BEST);

    riverPositions_cu0.SetSourceModule (0, riverPositions_rm0);
    riverPositions_cu0.AddControlPoint (-2.000,  2.000);
    riverPositions_cu0.AddControlPoint (-1.000,  1.000);
    riverPositions_cu0.AddControlPoint (-0.125,  0.875);
    riverPositions_cu0.AddControlPoint ( 0.000, -1.000);
    riverPositions_cu0.AddControlPoint ( 1.000, -1.500);
    riverPositions_cu0.AddControlPoint ( 2.000, -2.000);

    riverPositions_rm1.SetSeed (pi.seed + 101);
    riverPositions_rm1.SetFrequency (43.25);
    riverPositions_rm1.SetLacunarity (pi.continentLacunarity);
    riverPositions_rm1.SetOctaveCount (1);
    riverPositions_rm1.SetNoiseQuality (QUALITY_BEST);

    riverPositions_cu1.SetSourceModule (0, riverPositions_rm1);
    riverPositions_cu1.AddControlPoint (-2.000,  2.0000);
    riverPositions_cu1.AddControlPoint (-1.000,  1.5000);
    riverPositions_cu1.AddControlPoint (-0.125,  1.4375);
    riverPositions_cu1.AddControlPoint ( 0.000,  0.5000);
    riverPositions_cu1.AddControlPoint ( 1.000,  0.2500);
    riverPositions_cu1.AddControlPoint ( 2.000,  0.0000);

    riverPositions_mi.SetSourceModule (0, riverPositions_cu0);
    riverPositions_mi.SetSourceModule (1, riverPositions_cu1);

    riverPositions_tu.SetSourceModule (0, riverPositions_mi);
    riverPositions_tu.SetSeed (pi.seed + 102);
    riverPositions_tu.SetFrequency (9.25);
    riverPositions_tu.SetPower (1.0 / 57.75);
    riverPositions_tu.SetRoughness (6);

    riverPositions.SetSourceModule (0, riverPositions_tu);

    scaledMountainousTerrain_sb0.SetSourceModule (0, mountainousTerrain);
    scaledMountainousTerrain_sb0.SetScale (0.125);
    scaledMountainousTerrain_sb0.SetBias (0.125);

    scaledMountainousTerrain_pe.SetSeed (pi.seed + 110);
    scaledMountainousTerrain_pe.SetFrequency (14.5);
    scaledMountainousTerrain_pe.SetPersistence (0.5);
    scaledMountainousTerrain_pe.SetLacunarity (pi.mountainLacunarity);
    scaledMountainousTerrain_pe.SetOctaveCount (6);
    scaledMountainousTerrain_pe.SetNoiseQuality (QUALITY_STD);

    scaledMountainousTerrain_ex.SetSourceModule (0, scaledMountainousTerrain_pe);
    scaledMountainousTerrain_ex.SetExponent (1.25);

    scaledMountainousTerrain_sb1.SetSourceModule (0,
            scaledMountainousTerrain_ex);
    scaledMountainousTerrain_sb1.SetScale (0.25);
    scaledMountainousTerrain_sb1.SetBias (1.0);

    scaledMountainousTerrain_mu.SetSourceModule (0,
            scaledMountainousTerrain_sb0);
    scaledMountainousTerrain_mu.SetSourceModule (1,
            scaledMountainousTerrain_sb1);

    scaledMountainousTerrain.SetSourceModule (0, scaledMountainousTerrain_mu);

    scaledHillyTerrain_sb0.SetSourceModule (0, hillyTerrain);
    scaledHillyTerrain_sb0.SetScale (0.0625);
    scaledHillyTerrain_sb0.SetBias (0.0625);

    scaledHillyTerrain_pe.SetSeed (pi.seed + 120);
    scaledHillyTerrain_pe.SetFrequency (13.5);
    scaledHillyTerrain_pe.SetPersistence (0.5);
    scaledHillyTerrain_pe.SetLacunarity (pi.hillsLacunarity);
    scaledHillyTerrain_pe.SetOctaveCount (6);
    scaledHillyTerrain_pe.SetNoiseQuality (QUALITY_STD);

    scaledHillyTerrain_ex.SetSourceModule (0, scaledHillyTerrain_pe);
    scaledHillyTerrain_ex.SetExponent (1.25);

    scaledHillyTerrain_sb1.SetSourceModule (0, scaledHillyTerrain_ex);
    scaledHillyTerrain_sb1.SetScale (0.5);
    scaledHillyTerrain_sb1.SetBias (1.5);

    scaledHillyTerrain_mu.SetSourceModule (0, scaledHillyTerrain_sb0);
    scaledHillyTerrain_mu.SetSourceModule (1, scaledHillyTerrain_sb1);

    scaledHillyTerrain.SetSourceModule (0, scaledHillyTerrain_mu);

    scaledPlainsTerrain_sb.SetSourceModule (0, plainsTerrain);
    scaledPlainsTerrain_sb.SetScale (0.00390625);
    scaledPlainsTerrain_sb.SetBias (0.0078125);

    scaledPlainsTerrain.SetSourceModule (0, scaledPlainsTerrain_sb);

    scaledBadlandsTerrain_sb.SetSourceModule (0, badlandsTerrain);
    scaledBadlandsTerrain_sb.SetScale (0.0625);
    scaledBadlandsTerrain_sb.SetBias (0.0625);

    scaledBadlandsTerrain.SetSourceModule (0, scaledBadlandsTerrain_sb);

    continentalShelf_te.SetSourceModule (0, continentDef);
    continentalShelf_te.AddControlPoint (-1.0);
    continentalShelf_te.AddControlPoint (-0.75);
    continentalShelf_te.AddControlPoint (pi.shelfLevel);
    continentalShelf_te.AddControlPoint (1.0);

    continentalShelf_rm.SetSeed (pi.seed + 130);
    continentalShelf_rm.SetFrequency (pi.continentFrequency * 4.375);
    continentalShelf_rm.SetLacunarity (pi.continentLacunarity);
    continentalShelf_rm.SetOctaveCount (16);
    continentalShelf_rm.SetNoiseQuality (QUALITY_BEST);

    continentalShelf_sb.SetSourceModule (0, continentalShelf_rm);
    continentalShelf_sb.SetScale (-0.125);
    continentalShelf_sb.SetBias (-0.125);

    continentalShelf_cl.SetSourceModule (0, continentalShelf_te);
    continentalShelf_cl.SetBounds (-0.75, pi.seaLevel);

    continentalShelf_ad.SetSourceModule (0, continentalShelf_sb);
    continentalShelf_ad.SetSourceModule (1, continentalShelf_cl);

    continentalShelf.SetSourceModule (0, continentalShelf_ad);

    baseContinentElev_sb.SetSourceModule (0, continentDef);
    baseContinentElev_sb.SetScale (pi.continentHeightScale);
    baseContinentElev_sb.SetBias (0.0);

    baseContinentElev_se.SetSourceModule (0, baseContinentElev_sb);
    baseContinentElev_se.SetSourceModule (1, continentalShelf);
    baseContinentElev_se.SetControlModule (continentDef);
    baseContinentElev_se.SetBounds (pi.shelfLevel - 1000.0, pi.shelfLevel);
    baseContinentElev_se.SetEdgeFalloff (0.03125);

    baseContinentElev.SetSourceModule (0, baseContinentElev_se);

    continentsWithPlains_ad.SetSourceModule (0, baseContinentElev);
    continentsWithPlains_ad.SetSourceModule (1, scaledPlainsTerrain);

    continentsWithPlains.SetSourceModule (0, continentsWithPlains_ad);

    continentsWithHills_ad.SetSourceModule (0, baseContinentElev);
    continentsWithHills_ad.SetSourceModule (1, scaledHillyTerrain);

    continentsWithHills_se.SetSourceModule (0, continentsWithPlains);
    continentsWithHills_se.SetSourceModule (1, continentsWithHills_ad);
    continentsWithHills_se.SetControlModule (terrainTypeDef);
    continentsWithHills_se.SetBounds (1.0 - pi.hillsAmount, 1001.0 - pi.hillsAmount);
    continentsWithHills_se.SetEdgeFalloff (0.25);

    continentsWithHills.SetSourceModule (0, continentsWithHills_se);

    continentsWithMountains_ad0.SetSourceModule (0, baseContinentElev);
    continentsWithMountains_ad0.SetSourceModule (1, scaledMountainousTerrain);

    continentsWithMountains_cu.SetSourceModule (0, continentDef);
    continentsWithMountains_cu.AddControlPoint (                  -1.0, -0.0625);
    continentsWithMountains_cu.AddControlPoint (                   0.0,  0.0000);
    continentsWithMountains_cu.AddControlPoint (1.0 - pi.mountainsAmount,  0.0625);
    continentsWithMountains_cu.AddControlPoint (                   1.0,  0.2500);

    continentsWithMountains_ad1.SetSourceModule (0, continentsWithMountains_ad0);
    continentsWithMountains_ad1.SetSourceModule (1, continentsWithMountains_cu);

    continentsWithMountains_se.SetSourceModule (0, continentsWithHills);
    continentsWithMountains_se.SetSourceModule (1, continentsWithMountains_ad1);
    continentsWithMountains_se.SetControlModule (terrainTypeDef);
    continentsWithMountains_se.SetBounds (1.0 - pi.mountainsAmount,
            1001.0 - pi.mountainsAmount);
    continentsWithMountains_se.SetEdgeFalloff (0.25);

    continentsWithMountains.SetSourceModule (0, continentsWithMountains_se);

    continentsWithBadlands_pe.SetSeed (pi.seed + 140);
    continentsWithBadlands_pe.SetFrequency (16.5);
    continentsWithBadlands_pe.SetPersistence (0.5);
    continentsWithBadlands_pe.SetLacunarity (pi.continentLacunarity);
    continentsWithBadlands_pe.SetOctaveCount (2);
    continentsWithBadlands_pe.SetNoiseQuality (QUALITY_STD);

    continentsWithBadlands_ad.SetSourceModule (0, baseContinentElev);
    continentsWithBadlands_ad.SetSourceModule (1, scaledBadlandsTerrain);

    continentsWithBadlands_se.SetSourceModule (0, continentsWithMountains);
    continentsWithBadlands_se.SetSourceModule (1, continentsWithBadlands_ad);
    continentsWithBadlands_se.SetControlModule (continentsWithBadlands_pe);
    continentsWithBadlands_se.SetBounds (1.0 - pi.badlandsAmount,
            1001.0 - pi.badlandsAmount);
    continentsWithBadlands_se.SetEdgeFalloff (0.25);

    continentsWithBadlands_ma.SetSourceModule (0, continentsWithMountains);
    continentsWithBadlands_ma.SetSourceModule (1, continentsWithBadlands_se);

    continentsWithBadlands.SetSourceModule (0, continentsWithBadlands_ma);

    continentsWithRivers_sb.SetSourceModule (0, riverPositions);
    continentsWithRivers_sb.SetScale (pi.riverDepth / 2.0);
    continentsWithRivers_sb.SetBias (-pi.riverDepth / 2.0);

    continentsWithRivers_ad.SetSourceModule (0, continentsWithBadlands);
    continentsWithRivers_ad.SetSourceModule (1, continentsWithRivers_sb);

    continentsWithRivers_se.SetSourceModule (0, continentsWithBadlands);
    continentsWithRivers_se.SetSourceModule (1, continentsWithRivers_ad);
    continentsWithRivers_se.SetControlModule (continentsWithBadlands);
    continentsWithRivers_se.SetBounds (pi.seaLevel,
            pi.continentHeightScale + pi.seaLevel);
    continentsWithRivers_se.SetEdgeFalloff (pi.continentHeightScale - pi.seaLevel);

    continentsWithRivers.SetSourceModule (0, continentsWithRivers_se);
}

PlanetGenerator::~PlanetGenerator()
{

}
