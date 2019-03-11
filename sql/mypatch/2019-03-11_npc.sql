/*为花丛蛇设置被动法术，增强恐惧，待测试*/
update creature_template set spell1=6243 where entry=3680;

/*瘟疫软泥（蓝）变形*/
update creature_template set spell1=28988 where entry=16783;

/*瘟疫软泥（红）变形*/
update creature_template set spell1=28987 where entry=16784;

/*天灾软泥（绿）变形*/
update creature_template set spell1=28990 where entry=16785;
update locales_creature set name_loc4='瘟疫软泥（绿）' where entry=16785;

/*天灾软泥怪（黑）变形*/
update creature_template set spell1=28989 where entry=16243;
update locales_creature set name_loc4='瘟疫软泥（黑）' where entry=16243;