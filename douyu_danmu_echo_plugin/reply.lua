function inner_on_chat_msg(user,msg,deserveLev)
	return user .. "[级别 " .. deserveLev .. "]: " .. msg
end

function inner_on_user_gift(user,gift,number)
	return user .. " 赠送了" .. number .. "个: " .. gift
end

function inner_on_user_donater(user,sliver)
	return user .. " 赠送了" .. sliver .. "鱼丸"
end

function inner_on_user_deserve(user,num,level)
	return user .. "[level " .. level .. "]: 赠送了酬勤" .. num .. "个"
end

function inner_on_user_enter(user,level)
	return user .. "[level " .. level .. "]: 欢迎光临陋室."
end

function inner_on_automsg()
	return "大家早上好，主播正在写一个斗鱼弹幕echo插件，采用lua作为脚本。"
end

--sec
function inner_get_interval_time()
	return 100
end