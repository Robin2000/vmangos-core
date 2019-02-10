#https://github.com/cmangos/issues/wiki/dbscripts
#1657 臭气熏天的南海镇
#item=20387 亡灵臭气弹
#item点击施法24706投掷臭气弹
#spell施放触发事件Send Event (9417)
#event_scripts.ID为事件id
#command=8为完成任务杀死creature
#datalong=15415是该任务对应的ReqCreatureOrGoId1
#datalong2=1表示允许组队完成任务

insert into event_scripts(id,command,datalong,datalong2,comments) values(9417,8,15415,1,'臭气熏天的南海镇');


#3514 部落的示威
INSERT INTO gameobject
   (`guid`, `id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecsmin`, `spawntimesecsmax`, `animprogress`, `state`, `spawnFlags`, `visibilitymod`, `patch_min`, `patch_max`)
VALUES
   (3998643, 152093, 1, 2371.22, 1796.58, 365.068, 0.3163, 0, 0, 0.157491, 0.98752, 25, 25, 100, 1, 0, 0, 0, 10);