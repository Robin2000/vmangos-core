/*为花丛蛇设置被动法术，增强恐惧，待测试*/
update creature_template set spell_id1=6243,script_name='generic_spell_ai' where entry=3680 AND (script_name='' OR script_name IS NULL);

