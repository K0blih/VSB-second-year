-- 1. VLOŽENÍ UŽIVATELŮ (USERS)
INSERT INTO [users] ([username], [email], [display_name]) VALUES
('Arthas', 'arthas@wow.com', 'Lich King'),
('Thrall', 'thrall@horde.org', 'Warchief'),
('Jaina', 'jaina@kirintor.dalaran', 'Lady Proudmoore'),
('Illidan', 'illidan@outland.net', 'Stormrage'),
('Sylvanas', 'sylvanas@undercity.com', 'Banshee Queen');
GO

-- 2. VLOŽENÍ GUILD (GUILDS)
-- Guilda 1: "The Scourge" (Vůdce: Arthas [ID 1])
-- Guilda 2: "The Horde" (Vůdce: Thrall [ID 2])
INSERT INTO [guilds] ([name], [tag], [description], [leader_user_id]) VALUES
('The Scourge', 'DEAD', 'We serve the Frozen Throne.', 1),
('The Horde', 'ZUG', 'Strength and Honor.', 2);
GO

-- 3. VLOŽENÍ ROLÍ (GUILD_ROLES)
-- Role pro Guildu 1 (The Scourge - ID 1)
INSERT INTO [guild_roles] ([guild_id], [name], [permissions], [rank]) VALUES
(1, 'Lich King', 'ALL_PERMISSIONS', 100),
(1, 'Death Knight', 'INVITE, KICK, BANK_ACCESS', 50),
(1, 'Ghoul', 'NO_PERMISSIONS', 0);

-- Role pro Guildu 2 (The Horde - ID 2)
INSERT INTO [guild_roles] ([guild_id], [name], [permissions], [rank]) VALUES
(2, 'Warchief', 'ALL_PERMISSIONS', 100),
(2, 'Shaman', 'INVITE, BANK_DEPOSIT', 60),
(2, 'Peon', 'NO_PERMISSIONS', 0);
GO

-- 4. VLOŽENÍ ČLENŮ (MEMBERS)
INSERT INTO [members] ([guild_id], [user_id], [role_id]) VALUES (1, 1, 1);

INSERT INTO [members] ([guild_id], [user_id], [role_id]) VALUES (1, 3, 2);

INSERT INTO [members] ([guild_id], [user_id], [role_id]) VALUES (1, 4, 3);

INSERT INTO [members] ([guild_id], [user_id], [role_id]) VALUES (2, 2, 4);

INSERT INTO [members] ([guild_id], [user_id], [role_id]) VALUES (2, 5, 5);
GO

-- 5. VLOŽENÍ INVENTÁŘE (GUILD_INVENTORY)
-- Naplníme banku guildy "The Scourge" (ID 1)
INSERT INTO [guild_inventory] ([guild_id], [item_name], [quantity], [item_type]) VALUES
(1, 'Frostmourne Shard', 5, 'Material'),
(1, 'Health Potion', 100, 'Consumable'),
(1, 'Rusty Sword', 20, 'Weapon');

-- Naplníme banku guildy "The Horde" (ID 2)
INSERT INTO [guild_inventory] ([guild_id], [item_name], [quantity], [item_type]) VALUES
(2, 'Wolf Meat', 500, 'Food'),
(2, 'Axe of Orgrimmar', 2, 'Weapon');
GO

-- 6. VLOŽENÍ AUDIT LOGŮ (GUILD_AUDIT_LOG)
-- Arthas vložil 100 potionů
INSERT INTO [guild_audit_log] ([guild_id], [user_id], [action_type], [item_name], [amount], [note]) VALUES
(1, 1, 'DEPOSIT', 'Health Potion', 100, 'Loot from dungeon');

-- Jaina vybrala 1 meč
INSERT INTO [guild_audit_log] ([guild_id], [user_id], [action_type], [item_name], [amount], [note]) VALUES
(1, 3, 'WITHDRAW', 'Rusty Sword', -1, 'Need for leveling');

-- Thrall vložil jídlo
INSERT INTO [guild_audit_log] ([guild_id], [user_id], [action_type], [item_name], [amount], [note]) VALUES
(2, 2, 'DEPOSIT', 'Wolf Meat', 500, 'Hunting session');
GO

--SELECT 
--    g.name AS Guild,
--    u.username AS Member,
--    r.name AS Rank,
--    r.permissions
--FROM members m
--JOIN users u ON m.user_id = u.id
--JOIN guilds g ON m.guild_id = g.id
--JOIN guild_roles r ON m.role_id = r.id
--ORDER BY g.name, r.rank DESC;