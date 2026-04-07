var config = {
    type: Phaser.AUTO,
    scale: {
        parent: 'phaser-app',
        mode: Phaser.Scale.FIT,
        autoCenter: Phaser.Scale.CENTER_BOTH,
        width: 1400,
        height: 800
    },
    physics: {
        default: 'arcade',
        arcade: {
            gravity: { y: 0 },
            debug: false
        }
    },
    scene: {
        preload: preload,
        create: create,
        update: update
    },
    debug: true
};

var game = new Phaser.Game(config);

var backgroundLayer;
var collisionLayer;
var objectsLayer;
var map;
var hud;
var player;
var bombs;
var robot;
var collectible;
var cursors;
var move_ctl = false;
var left;
var right;
var up;
var down;

var score = 0;
var bestScore = 0;
var itemTimeoutSeconds = 7;
var nextTimeoutAt = 0;

function preload() {
    this.load.spritesheet('robot', 'assets/lego.png',
        { frameWidth: 37, frameHeight: 48 });
    this.load.spritesheet('player', 'assets/warrior_spritesheet_calciumtrice.png',
        { frameWidth: 32, frameHeight: 32 });
    this.load.spritesheet('items', 'assets/FinishedB.png',
        { frameWidth: 32, frameHeight: 32 });
    this.load.image('bomb', 'assets/bomb.png');
    this.load.image('tiles', 'assets/mountain_landscape.png');
    this.load.tilemapTiledJSON('json_map', 'assets/json_map.json');
}

function resize(width, height) {
    if (!game || !game.scale) {
        return;
    }

    game.scale.refresh();
    fitCameraToMap();
    positionHud();
}

function create() {
    map = this.make.tilemap({ key: 'json_map' });
    var tiles = map.addTilesetImage('map_tiles', 'tiles');

    collisionLayer = map.createLayer('collision', tiles, 0, 0);
    backgroundLayer = map.createLayer('background', tiles, 0, 0);
    objectsLayer = map.createLayer('objects', tiles, 0, 0);
    collisionLayer.setVisible(false);
    collisionLayer.setCollisionByExclusion([-1]);

    this.physics.world.setBounds(0, 0, map.widthInPixels, map.heightInPixels);
    this.cameras.main.setBounds(0, 0, map.widthInPixels, map.heightInPixels);

    player = this.physics.add.sprite(96, 96, 'player');
    player.setCollideWorldBounds(true);
    player.setSize(20, 24).setOffset(6, 6);

    robot = this.physics.add.sprite(224, 224, 'robot');
    robot.setCollideWorldBounds(true);
    robot.setBounce(0, 0);

    this.physics.add.collider(player, collisionLayer);
    this.physics.add.collider(robot, collisionLayer);
    this.cameras.main.roundPixels = true;
    fitCameraToMap();

    collectible = this.physics.add.sprite(0, 0, 'items', 0);
    collectible.body.setAllowGravity(false);
    collectible.setImmovable(true);
    collectible.setDepth(1);

    bombs = this.physics.add.group();
    createBombs.call(this, 4);

    this.physics.add.collider(bombs, collisionLayer);
    this.physics.add.collider(bombs, bombs);
    this.physics.add.overlap(player, collectible, collectItem, null, this);
    this.physics.add.overlap(player, bombs, hitBomb, null, this);
    this.physics.add.overlap(player, robot, hitRobot, null, this);
    hud = document.getElementById('hud');

    this.anims.create({
        key: 'walk',
        frames: this.anims.generateFrameNumbers('player', { start: 11, end: 19 }),
        frameRate: 10,
        repeat: -1
    });

    this.anims.create({
        key: 'robot_run',
        frames: this.anims.generateFrameNumbers('robot', { start: 0, end: 15 }),
        frameRate: 12,
        repeat: -1
    });
    robot.anims.play('robot_run', true);

    this.anims.create({
        key: 'idle',
        frames: this.anims.generateFrameNumbers('player', { start: 1, end: 9 }),
        frameRate: 10,
        repeat: -1
    });

    cursors = this.input.keyboard.createCursorKeys();

    this.input.on('pointerdown', function(pointer) {
        move_ctl = true;
        pointer_move(pointer);
    });
    this.input.on('pointerup', function() {
        move_ctl = false;
        reset_move();
    });
    this.input.on('pointermove', pointer_move);

    window.addEventListener('resize', function() {
        resize(window.innerWidth, window.innerHeight);
    }, false);
    resize(window.innerWidth, window.innerHeight);

    spawnCollectible.call(this);
    updateText.call(this);
}

function positionHud() {
    if (!hud || !game || !game.canvas) {
        return;
    }

    var rect = game.canvas.getBoundingClientRect();
    hud.style.left = (rect.left + rect.width / 2 - hud.offsetWidth / 2) + 'px';
    hud.style.top = (rect.top + 16) + 'px';
}

function fitCameraToMap() {
    if (!game || !game.scene || !game.scene.scenes.length || !map) {
        return;
    }

    var camera = game.scene.scenes[0].cameras.main;
    var zoomX = camera.width / map.widthInPixels;
    var zoomY = camera.height / map.heightInPixels;
    var zoom = Math.min(zoomX, zoomY);

    camera.stopFollow();
    camera.setZoom(zoom);
    camera.centerOn(map.widthInPixels / 2, map.heightInPixels / 2);
}

function createBombs(count) {
    for (var i = 0; i < count; i += 1) {
        var bombPosition = findSpawnPosition();
        var bomb = bombs.create(bombPosition.x, bombPosition.y, 'bomb');

        bomb.setCollideWorldBounds(true);
        bomb.setBounce(1, 1);
        bomb.setVelocity(
            Phaser.Math.Between(-180, 180),
            Phaser.Math.Between(-180, 180)
        );

        if (Math.abs(bomb.body.velocity.x) < 80) {
            bomb.body.velocity.x = 80 * (Math.random() < 0.5 ? -1 : 1);
        }
        if (Math.abs(bomb.body.velocity.y) < 80) {
            bomb.body.velocity.y = 80 * (Math.random() < 0.5 ? -1 : 1);
        }
    }
}

function pointer_move(pointer) {
    var dx = 0;
    var dy = 0;
    var min_pointer = (player.body.width + player.body.height) / 4;

    if (move_ctl) {
        reset_move();

        dx = (pointer.worldX - player.x);
        dy = (pointer.worldY - player.y);

        if (Math.abs(dx) > min_pointer) {
            left = (dx < 0);
            right = !left;
        } else {
            left = false;
            right = false;
        }

        if (Math.abs(dy) > min_pointer) {
            up = (dy < 0);
            down = !up;
        } else {
            up = false;
            down = false;
        }
    }
}

function reset_move() {
    up = false;
    down = false;
    left = false;
    right = false;
}

function update(time) {
    var move_x = true;
    var robotSpeed = 55;

    if (move_ctl) {
        pointer_move(game.input.activePointer);
    }

    if (cursors.left.isDown || left) {
        player.body.setVelocityX(-150);
        player.angle = 90;
        player.anims.play('walk', true);
    } else if (cursors.right.isDown || right) {
        player.body.setVelocityX(150);
        player.angle = 270;
        player.anims.play('walk', true);
    } else {
        player.body.setVelocityX(0);
        move_x = false;
    }

    if (cursors.up.isDown || up) {
        player.body.setVelocityY(-150);
        player.angle = 180;
        player.anims.play('walk', true);
    } else if (cursors.down.isDown || down) {
        player.body.setVelocityY(150);
        player.angle = 0;
        player.anims.play('walk', true);
    } else {
        player.body.setVelocityY(0);
        if (!move_x) {
            player.anims.play('idle', true);
        }
    }

    if (time >= nextTimeoutAt) {
        resetScore.call(this, 'Timeout');
        spawnCollectible.call(this);
    }

    this.physics.moveToObject(robot, player, robotSpeed);
    robot.angle = Phaser.Math.RadToDeg(
        Phaser.Math.Angle.Between(robot.x, robot.y, player.x, player.y)
    ) - 90;

    updateText.call(this, time);
}

function updateText(time) {
    if (hud) {
        hud.textContent = 'Items picked: ' + score;
    }
    positionHud();
}

function collectItem(playerSprite, item) {
    score += 1;
    if (score > bestScore) {
        bestScore = score;
    }

    spawnCollectible.call(this);
    updateText.call(this);
}

function hitBomb() {
    resetScore.call(this, 'Bomb');
}

function hitRobot() {
    resetScore.call(this, 'Robot');
}

function resetScore(reason) {
    if (score > bestScore) {
        bestScore = score;
    }

    score = 0;
    updateText.call(this);
}

function spawnCollectible() {
    var position = findSpawnPosition();
    var frame = Phaser.Math.Between(0, collectible.texture.frameTotal - 2);

    collectible.enableBody(true, position.x, position.y, true, true);
    collectible.setFrame(frame);
    nextTimeoutAt = this.time.now + itemTimeoutSeconds * 1000;
}

function findSpawnPosition() {
    var tileX;
    var tileY;
    var tile;
    var x;
    var y;
    var valid;

    do {
        valid = true;
        tileX = Phaser.Math.Between(0, map.width - 1);
        tileY = Phaser.Math.Between(0, map.height - 1);
        x = tileX * map.tileWidth + map.tileWidth / 2;
        y = tileY * map.tileHeight + map.tileHeight / 2;
        tile = collisionLayer ? collisionLayer.getTileAt(tileX, tileY) : null;

        if (tile !== null) {
            valid = false;
        }

        if (valid && player && Phaser.Math.Distance.Between(x, y, player.x, player.y) < map.tileWidth * 2) {
            valid = false;
        }

        if (valid && bombs) {
            bombs.children.iterate(function(bomb) {
                if (bomb && Phaser.Math.Distance.Between(x, y, bomb.x, bomb.y) < map.tileWidth * 1.5) {
                    valid = false;
                }
            });
        }
    } while (!valid);

    return {
        x: x,
        y: y
    };
}
