/* 1. USERS TABLE */
CREATE TABLE [users] (
  [id] int PRIMARY KEY IDENTITY(1, 1),
  [username] varchar(50) UNIQUE NOT NULL,
  [email] varchar(100) UNIQUE NOT NULL,
  [display_name] varchar(100),
  [joined_at] datetime2 DEFAULT (CURRENT_TIMESTAMP)
)
GO

/* 2. GUILDS TABLE */
CREATE TABLE [guilds] (
  [id] int PRIMARY KEY IDENTITY(1, 1),
  [name] varchar(100) UNIQUE NOT NULL,
  [tag] varchar(5),
  [description] varchar(max),
  [created_at] datetime2 DEFAULT (CURRENT_TIMESTAMP),
  [leader_user_id] int
)
GO

/* 3. GUILD ROLES TABLE */
CREATE TABLE [guild_roles] (
  [id] int PRIMARY KEY IDENTITY(1, 1),
  [guild_id] int,
  [name] varchar(50) NOT NULL,
  [permissions] varchar(max),
  [rank] int NOT NULL DEFAULT (0) 
)
GO

/* 4. MEMBERS TABLE */
CREATE TABLE [members] (
  [guild_id] int,
  [user_id] int,
  [role_id] int,
  [joined_at] datetime2 DEFAULT (CURRENT_TIMESTAMP),
  CONSTRAINT [PK_members] PRIMARY KEY (guild_id, user_id)
)
GO

/* 5. GUILD INVENTORY TABLE */
CREATE TABLE [guild_inventory] (
  [id] int PRIMARY KEY IDENTITY(1, 1),
  [guild_id] int,
  [item_name] varchar(100) NOT NULL,
  [quantity] int NOT NULL DEFAULT (0),
  [item_type] varchar(50),
  [last_updated] datetime2 DEFAULT (CURRENT_TIMESTAMP),
  CONSTRAINT [CK_Inventory_Quantity] CHECK ([quantity] >= 0)
)
GO

/* 6. GUILD AUDIT LOG TABLE */
CREATE TABLE [guild_audit_log] (
  [id] int PRIMARY KEY IDENTITY(1, 1),
  [guild_id] int,
  [user_id] int,
  [action_type] varchar(20) NOT NULL,
  [item_name] varchar(100),
  [amount] int,
  [note] varchar(max),
  [created_at] datetime2 DEFAULT (CURRENT_TIMESTAMP)
)
GO

/* --- INDEXES --- */
CREATE UNIQUE INDEX [guild_roles_index_0] ON [guild_roles] ([guild_id], [name])
GO

CREATE UNIQUE INDEX [guild_inventory_index_1] ON [guild_inventory] ([guild_id], [item_name])
GO

/* --- FOREIGN KEYS --- */

-- Guild Leader link
ALTER TABLE [guilds] ADD CONSTRAINT [FK_Guilds_Leader] 
FOREIGN KEY ([leader_user_id]) REFERENCES [users] ([id])
GO

-- Roles belong to Guilds
ALTER TABLE [guild_roles] ADD CONSTRAINT [FK_Roles_Guild] 
FOREIGN KEY ([guild_id]) REFERENCES [guilds] ([id])
GO

-- Member links
ALTER TABLE [members] ADD CONSTRAINT [FK_Members_Guild] 
FOREIGN KEY ([guild_id]) REFERENCES [guilds] ([id])
GO

ALTER TABLE [members] ADD CONSTRAINT [FK_Members_User] 
FOREIGN KEY ([user_id]) REFERENCES [users] ([id])
GO

ALTER TABLE [members] ADD CONSTRAINT [FK_Members_Role] 
FOREIGN KEY ([role_id]) REFERENCES [guild_roles] ([id])
GO

-- Inventory belongs to Guild
ALTER TABLE [guild_inventory] ADD CONSTRAINT [FK_Inventory_Guild] 
FOREIGN KEY ([guild_id]) REFERENCES [guilds] ([id])
GO

-- Audit Log links
ALTER TABLE [guild_audit_log] ADD CONSTRAINT [FK_Audit_Guild] 
FOREIGN KEY ([guild_id]) REFERENCES [guilds] ([id])
GO

ALTER TABLE [guild_audit_log] ADD CONSTRAINT [FK_Audit_User] 
FOREIGN KEY ([user_id]) REFERENCES [users] ([id])
GO