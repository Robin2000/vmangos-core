DROP PROCEDURE IF EXISTS add_migration;
delimiter ??
CREATE PROCEDURE `add_migration`()
BEGIN
DECLARE v INT DEFAULT 1;
SET v = (SELECT COUNT(*) FROM `migrations` WHERE `id`='20201118174855');
IF v=0 THEN
INSERT INTO `migrations` VALUES ('20201118174855');
-- Add your query below.


-- Correct scale of cats and dogs in Arathi Basin.
UPDATE `creature_template` SET `display_scale1`=0 WHERE `entry` IN (15065, 15066, 15071);


-- End of migration.
END IF;
END??
delimiter ; 
CALL add_migration();
DROP PROCEDURE IF EXISTS add_migration;
