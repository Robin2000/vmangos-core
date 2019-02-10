#1657 臭气熏天的南海镇
#item=20387 亡灵臭气弹
#item点击施法24706投掷臭气弹
#spell施放触发事件Send Event (9417)
#event_scripts.ID为事件id
#command=8为完成任务杀死creature
#datalong=15415是该任务对应的ReqCreatureOrGoId1
#datalong2=1表示允许组队完成任务

insert into event_scripts(id,command,datalong,datalong2,comments) values(9417,8,15415,1,'臭气熏天的南海镇');
