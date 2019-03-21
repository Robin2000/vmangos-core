罗卡鲁to雷克萨

update locales_creature set name_loc4='雷克萨' where entry=10182;
update locales_item set name_loc4='雷克萨的证明信' where entry=16785;

update locales_quest set title_loc4='雷克萨的证明信' where entry=6568;
update locales_quest set Objectives_loc4='把雷克萨的信交给西瘟疫之地的巫女麦兰达。' where entry=6568;
update locales_quest set offerRewardText_loc4='<米兰达拿着便笺看了看，然後笑了笑。>$B$B他最近怎麽样了?我说的是雷克萨。哦，过来，不要感到奇怪。这张信纸是故意留着空白的，其中包含了雷克萨的意图和想法。从其中的内容来看，无疑是雷克萨故意这麽做的。如果你被抓了或者被杀了的话，其中的资讯也不会泄露!$B$B哦，不要介意!米兰达会帮你的，$r。我欠酋长的情。$B$B<米兰达微笑着。>$B' where entry=6568;

update locales_quest set Objectives_loc4='看来这场假面舞会就要结束了。你知道麦兰达为你制作的龙形护符在黑石塔里面不会发挥作用，也许你应该去找雷克萨，将你的困境告诉他。把黯淡的龙火护符给他看看，也许他知道下一步该怎么做。' where entry=6601;

update locales_quest set Objectives_loc4='到黑石塔去杀掉达基萨斯将军，把它的血交给雷克萨。' where entry=6602;

update locales_quest set Objectives_loc4='到黑石塔去杀掉达基萨斯将军，把它的血交给雷克萨。' where entry=6567;

update locales_broadcast_text set MaleText_loc4='你好，$c。我是雷克萨。' where id=9013;
update locales_broadcast_text set MaleText_loc4='雷克萨，我丢了龙火护符。你能再给我一个吗？' where id=9014;
update locales_broadcast_text set FemaleText_loc4='雷克萨，我丢了龙火护符。你能再给我一个吗？' where id=9014;


update locales_gossip_menu_option set option_text_loc4='雷克萨，我把龙火护符弄丢了。可以再创造一个吗?' where menu_id=10182 and id=0;




