offset=40;
int16(sat);
int16(raw);


t=offset:1/8:offset+40 -1/8;

x=sat;
max_x=max(x);
min_x=min(x);
index_max=find(x==max_x);
index_min=find(x==min_x);


if (size(index_max,2)>1)

	first_index=index_max(1);
	for i = 2:size(index_max,2)
		if (index_max(i)==index_max(i-1)+1)
			last_index=index_max(i);
		else
			y=polyfit([first_index-2 first_index-1 last_index+1 last_index+2],[x(first_index-2) sat(first_index-1) sat(first_index+1) sat(first_index+2)],2);
	 		z=polyval(y,first_index:last_index);
	 		x(first_index:last_index)=z;
			first_index=index_max(i);
		end

	end

	 
	 y=polyfit([first_index-2 first_index-1 last_index+1 last_index+2],[x(first_index-2) sat(first_index-1) sat(first_index+1) sat(first_index+2)],2);
	 z=polyval(y,first_index:last_index);
	 x(first_index:last_index)=z;

end


if (size(index_max,2)>1)

	first_index=index_min(1);
	for i = 2:size(index_min,2)
		if (index_min(i)==index_min(i-1)+1)
			last_index=index_min(i);
		else
			y=polyfit([first_index-2 first_index-1 last_index+1 last_index+2],[x(first_index-2) sat(first_index-1) sat(first_index+1) sat(first_index+2)],2);
	 		z=polyval(y,first_index:last_index);
	 		x(first_index:last_index)=z;
			first_index=index_min(i);
		end

	end
	y=polyfit([first_index-2 first_index-1 last_index+1 last_index+2],[x(first_index-2) sat(first_index-1) sat(first_index+1) sat(first_index+2)],2);
	z=polyval(y,first_index:last_index);
	x(first_index:last_index)=z;
end

int16(x);
