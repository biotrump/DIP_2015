% @param train : logical training image
% @param tbb : training image bounding box, y0,y1,x0,x1
% @param sample :logical sample image
% @param sbb : sample image bounding box, x0 y0 x1 y1
function minDist3(train, tbb, sample, sbb,sbb_n)
	nsbb=size(sbb,1);%x0 y0 x1 y1
	ntbb=size(tbb,1);%y0 y1 x0 x1

    %create the features of alphabet in the training set
    for j=1:26
        ty0 = tbb(j,1);
        ty1 = tbb(j,2);
        tx0 = tbb(j,3);
        tx1 = tbb(j,4);
        roi_t = train(ty0:ty1,tx0:tx1);
        [TA0(j), TAA(j,:), TP(j), TPA(j,:), TE4(j), TE8(j), TLA(j,:), TWA(j), TWR(j), THR(j),TC0(j)]=...
            get_feature(roi_t,'GRAY');
        fprintf('%d:[%c],tE4=%f, tE8=%f,tC0=%f,TWR=%f, THR=%f,TWA=%f\n',j, 'A'+j-1,TE4(j),TE8(j), TC0(j), TWR(j), THR(j),TWA(j));
    end
    %create the features of digit in training set
    for j=53:62
        ty0 = tbb(j,1);
        ty1 = tbb(j,2);
        tx0 = tbb(j,3);
        tx1 = tbb(j,4);
        roi_t = train(ty0:ty1,tx0:tx1);
        [TA0(j), TAA(j,:), TP(j), TPA(j,:), TE4(j), TE8(j), TLA(j,:), TWA(j), TWR(j), THR(j),TC0(j)]=...
            get_feature(roi_t,'GRAY');
        fprintf('%d:[%c],tE4=%f, tE8=%f,tC0=%f,TWR=%f, THR=%f,TWA=%f\n',j, 'A'+j-1,TE4(j),TE8(j), TC0(j), TWR(j), THR(j),TWA(j));
    end
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	for i=1:nsbb
		minval=inf;
		mI=0;
		sx0 = sbb(i,1);
		sy0 = sbb(i,2);
		sx1 = sbb(i,3);
		sy1 = sbb(i,4);

		roi_s = sample(sy0:sy1,sx0:sx1);
		[sA0, sAa, sP, sPa, sE4, sE8, sLa, sWa, sWr, sHr, sC0]=get_feature(roi_s,'GRAY');
        fprintf('++%d:[%c], sE4=%f, sE8=%f,sC0=%f, WR=%f,HR=%f,WA=%f\n',i, sbb(i,5), sE4,sE8, sC0, sWr, sHr,sWa);
        %checking the source roi to training set A-Z
		for j=1:26
            fprintf('%d:[%c],tE4=%f, tE8=%f,tC0=%f,TWR=%f, THR=%f,TWA=%f\n',j, 'A'+j-1,TE4(j),TE8(j), TC0(j), TWR(j), THR(j),TWA(j));
			%temp=(sE4-tE4)^2 +(sE8-tE8)^2+(sLa(1)-tLa(1))^2+(sLa(2)-tLa(2))^2+...
			%	(sWa-tWa)^2 + (sWr-tWr)^2 + (sHr-tHr)^2 + (sC0-tC0)^2;
            temp=(  (sC0-TC0(j)) / (max(TC0)- min(TC0))  )^2;% + (sE4-tE4)^2;
            t1=((sE8-TE8(j))/(max(TE8)-min(TE8)))^2;
            t2=( (sWa-TWA(j))/(max(TWA)-min(TWA)) )^2;
            %t2=( ( sWa-TWA(j) )/ (  max(TWA(j))-min(TWA(j) ) ) )^2;
            temp=temp/4+t1;%+t2;
            %temp=(sE8-TE8(j))^2;
			if(temp < minval)
				mI=j;
				minval=temp;
			end
        end
		% matched alphabet in training set
		ty0 = tbb(mI,1);
		ty1 = tbb(mI,2);
		tx0 = tbb(mI,3);
		tx1 = tbb(mI,4);
		roi_t = train(ty0:ty1,tx0:tx1);
		fprintf('>>i=%d:[%c], mI=%d, min=%f,\n',i, sbb(i,5), mI, minval);
		figure;
		subplot(1,2,1);imshow(roi_s);title('alphabet');
		subplot(1,2,2);imshow(roi_t);title('TrainingSet.raw');        
    end

	for i=1:size(sbb_n)
		minval=inf;
		mI=0;
		sx0 = sbb_n(i,1);
		sy0 = sbb_n(i,2);
		sx1 = sbb_n(i,3);
		sy1 = sbb_n(i,4);

		roi_s = sample(sy0:sy1,sx0:sx1);
		[sA0, sAa, sP, sPa, sE4, sE8, sLa, sWa, sWr, sHr, sC0]=get_feature(roi_s,'GRAY');
        fprintf('++%d:[%c], sE4=%f, sE8=%f,sC0=%f, WR=%f,HR=%f,WA=%f\n',i, sbb(i,5), sE4,sE8, sC0, sWr, sHr,sWa);

        %if 0-9 is better
		for j=53:62
            fprintf('%d:[%c],tE4=%f, tE8=%f,tC0=%f,TWR=%f, THR=%f,TWA=%f\n',j, '0'+j-53,TE4(j),TE8(j), TC0(j), TWR(j), THR(j),TWA(j));
            %temp=(sE4-tE4)^2 +(sE8-tE8)^2+(sLa(1)-tLa(1))^2+(sLa(2)-tLa(2))^2+...
			%	(sWa-tWa)^2 + (sWr-tWr)^2 + (sHr-tHr)^2 + (sC0-tC0)^2;
            temp=( (sC0-TC0(j))/( max(TC0)- min(TC0) ) )^2;
            t1=((sE8-TE8(j))/(max(TE8)-min(TE8)))^2;
            t2=( (sWa-TWA(j))/(max(TWA)-min(TWA)) )^2;
            temp=temp/4+t1+t2;
			%temp=(sC0-tC0)^2 + (sE4-tE4)^2;
			%(sAa(1)-tAa(1))^2  + (sE4-tE4)^2 + (sWr-tWr)^2 +...
			% (sHr-tHr)^2;%+(sE8-tE8)^2;
			if(temp < minval)
				mI=j;
				minval=temp;
			end
		end

		% matched alphabet in training set
		ty0 = tbb(mI,1);
		ty1 = tbb(mI,2);
		tx0 = tbb(mI,3);
		tx1 = tbb(mI,4);
		roi_t = train(ty0:ty1,tx0:tx1);
		fprintf('<<i=%d:[%c], mI=%d, min=%f,\n',i, sbb_n(i,5), mI, minval);
		figure;
		subplot(1,2,1);imshow(roi_s);title('digit');
		subplot(1,2,2);imshow(roi_t);title('TrainingSet.raw');
	end

end