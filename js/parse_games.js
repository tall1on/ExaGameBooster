'use strict';

const axios = require('axios');
const fs = require('fs');

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

                if (games.includes(exeName))
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

    let gamesCount = 0;
    let gamesList = `# Supported games
\rCurrently supported games: [SUPPORTED_GAMES]\n\r`;
    for (let game of games)
    {
        if (!exeToNameMap[game])
        {
            continue;
        }

        if (gamesList.includes(exeToNameMap[game]))
        {
            continue;
        }

        gamesList += `- ${exeToNameMap[game]}\n\r`;
        gamesCount++;
    }

    fs.writeFileSync('../supportedGames.md', gamesList.replace('[SUPPORTED_GAMES]', String(gamesCount)));
    fs.writeFileSync('../assets/games.json', JSON.stringify(games));
});
