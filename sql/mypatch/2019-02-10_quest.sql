#https://github.com/cmangos/issues/wiki/dbscripts
#1657 ����Ѭ����Ϻ���
#item=20387 ���������
#item���ʩ��24706Ͷ��������
#spellʩ�Ŵ����¼�Send Event (9417)
#event_scripts.IDΪ�¼�id
#command=8Ϊ�������ɱ��creature
#datalong=15415�Ǹ������Ӧ��ReqCreatureOrGoId1
#datalong2=1��ʾ��������������

insert into event_scripts(id,command,datalong,datalong2,comments) values(9417,8,15415,1,'����Ѭ����Ϻ���');


#3514 �����ʾ��
INSERT INTO gameobject
   (`guid`, `id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecsmin`, `spawntimesecsmax`, `animprogress`, `state`, `spawnFlags`, `visibilitymod`, `patch_min`, `patch_max`)
VALUES
   (3998643, 152093, 1, 2371.22, 1796.58, 365.068, 0.3163, 0, 0, 0.157491, 0.98752, 25, 25, 100, 1, 0, 0, 0, 10);