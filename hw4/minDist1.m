% @param train : logical training image
% @param tbb : training image bounding box, y0,y1,x0,x1
% @param sample :logical sample image
% @param sbb : sample image bounding box, x0 y0 x1 y1
function minDist1(train, tbb, sample, sbb)
	nsbb=size(sbb,1);%x0 y0 x1 y1
	ntbb=size(tbb,1);%y0 y1 x0 x1

	for i=1:nsbb
		min=inf;
		mI=0;
		sx0 = sbb(i,1);
		sy0 = sbb(i,2);
		sx1 = sbb(i,3);
		sy1 = sbb(i,4);

		roi_s = sample(sy0:sy1,sx0:sx1);
		[sA0, sAa, sP, sPa, sE4, sE8, sLa, sWa, sWr, sHr, sC0]=get_feature(roi_s,'GRAY');
		for j=1:26
			ty0 = tbb(j,1);
			ty1 = tbb(j,2);
			tx0 = tbb(j,3);
			tx1 = tbb(j,4);
			roi_t = train(ty0:ty1,tx0:tx1);
		    [tA0, tAa, tP, tPa, tE4, tE8, tLa, tWa, tWr, tHr,tC0]=get_feature(roi_t,'GRAY');
			temp=(sE4-tE4)^2 +(sE8-tE8)^2+(sLa(1)-tLa(1))^2+(sLa(2)-tLa(2))^2+...
				(sWa-tWa)^2 + (sWr-tWr)^2 + (sHr-tHr)^2 + (sC0-tC0)^2;
			if(temp < min)
				mI=j;
				min=temp;
			end
		end
		for j=52:61
			ty0 = tbb(j,1);
			ty1 = tbb(j,2);
			tx0 = tbb(j,3);
			tx1 = tbb(j,4);
			roi_t = train(ty0:ty1,tx0:tx1);
		    [tA0, tAa, tP, tPa, tE4, tE8, tLa, tWa, tWr, tHr,tC0]=get_feature(roi_t,'Duda');
			temp=(sC0-tC0)^2 + (sE4-tE4)^2;
			%(sAa(1)-tAa(1))^2  + (sE4-tE4)^2 + (sWr-tWr)^2 +...
			% (sHr-tHr)^2;%+(sE8-tE8)^2;
			if(temp < min)
				mI=j;
				min=temp;
			end
		end

		% matched alphabet in training set
		ty0 = tbb(mI,1);
		ty1 = tbb(mI,2);
		tx0 = tbb(mI,3);
		tx1 = tbb(mI,4);
		roi_t = train(ty0:ty1,tx0:tx1);
		fprintf('i=%d:%c, mI=%d, min=%f,\n',i, sbb(i,5), mI, min);
		figure;
		subplot(1,2,1);imshow(roi_s);title('Sample1.raw');
		subplot(1,2,2);imshow(roi_t);title('TrainingSet.raw');
	end

end