import { Provider } from '@angular/core';

export const environment = {
  production: true,
  game: true,
  urlProtocol: 'https',
  // Fallout Together has no public server list yet; connect by address.
  url: '',
  // No Fallout Together releases to compare against yet.
  githubUrl: '',
  overwriteVersion: "",
  chatMessageLengthLimit: 512,
  nbReconnectionAttempts: 5,
  // Most specific first; translations are written for Skyrim Together.
  textReplacements: [
    [
      'Skyrim.esm\nUpdate.esm\nDawnguard.esm\nHearthFires.esm\nDragonborn.esm\n_ResourcePack.esl\nSkyrimTogether.esp\nSkyrimTogetherQuestPatches.esp',
      'Fallout4.esm\nDLCRobot.esm\nDLCworkshop01.esm\nDLCCoast.esm\nDLCworkshop02.esm\nDLCworkshop03.esm\nDLCNukaWorld.esm',
    ],
    [
      'make sure to read the playguide on our wiki, which you can find through our website, <b>skyrim-together.com</b>.',
      'keep in mind that this is an early experimental build.',
    ],
    [
      'Do NOT connect to any server if you are still in the Helgen intro sequence. Make sure you just escaped Helgen first.',
      'Do NOT connect to any server before you have left Vault 111.',
    ],
    ['Anniversary Update', 'Creations'],
    ['Skyrim.ini', 'Fallout4.ini'],
    ['Skyrim Together', 'Fallout Together'],
    ['Skyrim', 'Fallout 4'],
  ] as [string, string][],

  providers: [] as Provider[],
};
