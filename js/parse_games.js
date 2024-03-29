'use strict';

const axios = require('axios');
const fs = require('fs');
const excludeGames = JSON.parse(fs.readFileSync('../assets/exclude_games.json'));

axios.get('https://discord.com/api/v10/applications/detectable').then(({status, data}) => {
    if (200 !== status) {
        throw new Error('Fetching games failed');
    }

    fs.writeFileSync('../assets/games_raw.json', JSON.stringify(data));

    const games = [];
    const exeToNameMap = {};
    for (let game of data)
    {
        if (game.executables) {
            for (let executable of game.executables)
            {
                if (executable.os !== 'win32')
                {
                    continue;
                }

                let exeName = executable.name.split('/').pop();

                if (games.includes(exeName) || excludeGames.includes(exeName))
                {
                    continue;
                }

                games.push(exeName);

                exeToNameMap[exeName] = game.name;
            }
        }
    }

    for (let game of JSON.parse(fs.readFileSync('../assets/additional_games.json')))
    {
        if (games.includes(game))
        {
            continue;
        }

        games.push(game);
    }

    const gameNames = [];
    let gamesCount = 0;
    let gamesList = `# Supported games
\rCurrently supported games: [SUPPORTED_GAMES]\n\rYou can now search the supported games on our website: [Search Games](https://exatek.de/products/exa-game-booster)`;
    for (let game of games)
    {
        if (!exeToNameMap[game])
        {
            continue;
        }

        if (gameNames.includes(exeToNameMap[game]))
        {
            continue;
        }

        gameNames.push(exeToNameMap[game]);
    }

    gameNames.sort();

    for (let gameName of gameNames)
    {
        gamesCount++;
    }

    fs.writeFileSync('../supportedGames.md', gamesList.replace('[SUPPORTED_GAMES]', String(gamesCount)));
    fs.writeFileSync('../assets/games.json', JSON.stringify(games));
    fs.writeFileSync('../assets/game_names.json', JSON.stringify(gameNames));
});
