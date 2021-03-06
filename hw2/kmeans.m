% @BRIEF Perform k-means clustering.
% Input:
%  @PARAM D:    3D matrix M * N * d, d is the dim of feature vector, M * N is the image dimension.
%  @PARAM init: k number of clusters or predefined label (1 x n vector) for each pixel
%  @RETURN label: cluster map of original image by row x col, M * N
%  @RETURN means: trained model structure
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
function [label, means, final_texture] = kmeans(D, init, zebra, cheetah, giraffe)
  %squeeze(D(i,j,:)); % feature vector as a column vector
  [row, col, depth]=size(D);
  % D, 3D matrix M * N * d => 2D matrix (M * N) * d => total M * N row vectors with dimension d
  X=reshape(D,[row*col depth]);  % feature vector is in row vector, there are total M * N vectors,
  X=X'; % row vector to colunm vector
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We can randomly pick K centers as a start condition, but how to choose randomly?
% Every pixel is assigned a random class, so K random centers are generated as the start condition.
%X: d x n data matrix, where n = M * N, total pixel number
	persistent my_count;
	if isempty(my_count)
	    my_count=1;
	end
	n = size(X,2);
	if numel(init)==1
	  k = init; % generating 1xn matrix which is randomly classified from 1,2,...k
	  label = ceil(k*rand(1,n));  % rand(1,n) generating 1 * n matrix whose value is [0.0-1.0]
	                            % label : 1xn whose value is 1 to k
	elseif numel(init)==n
	  label = init;   %predefined labels for each pixel in 1xn matrix
	end
	last = 0;
  while any(label ~= last)  %~= means "not equal", stop condition if K classes do not change.
    % u = column vector, all the unique numbers in the lable list
    %[C,ia,ic] = unique(A), If A is a vector, then C = A(ia) and A = C(ic).
    %C(ic) means to pick each element from ic as a index and use this index to pick a element in C,
    %so it generates a new list from C with the same numbers of elements of ic.
    % label = u(label(:)) => label(:) is a index list to pick the element in u to form
    % the label. Every element in label(:) should be from 1-k which is the elements in u, unique numbers.
    % if u has k unique number, label(:) and lable are the same.
    [u,~,label(:)] = unique(label);
    k = numel(u); % elements of u, how many classes are generated? It's K classes.

      % L = sparse(i,j,s,m,n,nzmax),  L(i(k),j(k)) = s(k)
      % A sparse matrix, L: A non-zero element is stored as (row,col), value,
      % where (row,col) is the index of matrix,
      % value is non-zero element in sparse matrix.
      % L is a sparse matrix, i, j, s  are vectors which have the same length
      % sparse matrix :( 1:n, label means (row,cols) rows is 1..n, col is label(1), label(2)... label(n)
      % 1,
      % n,k :n * k matrix, the sparse matrix
      % n): n is the max non-zero elements in the sparse matrix
      %%%%%%%%%%%%%%%%%%%%%%%%%%%
      % E is n * k class map matrix. n is the pixel number, k is the total classes.
      % If a pixel i is in class label(i), then (i, label(i)) is 1.
      % Since each pixel has only one class, the matrix is "sparse".
      E = sparse(1:n,label,1,n,k,n);  % transform label into indicator matrix

      %%%%%%%%%%%%%%%%%%%%%%%%%%%
      %A = spdiags(B,d,m,n) creates an m-by-n sparse matrix by taking
      %the columns of B and placing them along the diagonals specified by d.
      %http://www.mathworks.com/help/matlab/ref/spdiags.html?searchHighlight=spdiags
      %if d = 0, main diagonal
      %sum(E,1) : sum E along dim 1, ie. along "row", so result is row vector, 1 * k row vector
      %sum(E,1) is a row vector with k elements. each element is the sum of pixel numbers in the class.
      %spdiags( 1./sum(E,1)', 0, k , k) : A k*K diagonal matrix with its main diagonal are the 1/C_1, 1/C_2,1/C_3.. 1/C_k
      %where C_i is the pixel numbers in the class i.
      %E*spdiags( 1./sum(E,1)', 0, k , k) is a average matrix in each element if the element is non-zero.
      % X is d*n, n samples with d dimensions feature vectors
      % m = X*(   E*spdiags( 1./sum(E,1)', 0, k , k) ), m is the current K centers d * k
      m = X*(   E*spdiags( 1./sum(E,1)', 0, k , k) );    % compute k centers
      last = label;

      %%% update new label to the current k centers by eucleadian distance
      %bsxfun(fun,A,B) : Apply element-by-element binary operation to two arrays with singleton expansion enabled
      % operation is @minus, A - B element by element
      % m'*X  - dot(m,m,1)'/2) : dot(m,m,1) the qaure distance from the center to the origin
      %[val,label] = max(), val[] is the max value row vector, label(i) is the class index
      [val,label] = max( bsxfun(@minus, m'*X, dot(m,m,1)'/2 ) , [] , 1); % assign new labels
  end
  %k centers , each center is d dimension! ie, 4 x 9 or 4 x 25, 4 is class number,
  % 9 or 25 is the filter3x3 or filter5x5
  %If A is a matrix, then sum(A) returns a row vector containing the sum of each column.
%  energy = dot(X(:),X(:))-2*sum(val);
	%return k centers
  means = m;

	% reshape texture label 1xn to row * col as the original image
	map=reshape(label,[row col]);

	%setup 4 reference texture blocks for 3 animals: Zebra, Cheetah, Giraffe
	%and 1 background (y1:y2,x1:x2)
	giraffe_texture=map(220-10:220+10, 460-10:460+10);
	cheetah_texture = map(340-10:340+10, 180-10:180+10);
	zebra_texture=map(120-20:120+20,140-20:140+20);
	bg_texture=map(220-10:220+10,70-10:70+10);

	%the most frequent number in the reference block is the class number for the class
	m_bg = mode(mode(bg_texture));
	%label_bg=label;
	%label(label==m_bg)=0;
	%label(label==m_bg)=0;

	%assign the class 1 to cheetah
	m_cheetah = mode(mode(cheetah_texture));
	label_ch=map;
	%zero out all except the desired class, cheetah
	label_ch(label_ch ~=m_cheetah)=0;
	%assign the cheetah class#, 1
	label_ch(label_ch ==m_cheetah)=1;

	%assign the class 2 to giraffe
	m_giraffe = mode(mode(giraffe_texture));
	label_g=map;
	label_g(label_g~=m_giraffe)=0;
	label_g(label_g==m_giraffe)=2;

	%assign the class 3 to zebra
	m_zebra = mode(mode(zebra_texture));
	label_z=map;
	label_z(label_z~=m_zebra)=0;	%non zebra is set to 0
	label_z(label_z==m_zebra)=3;	%zebra class is set to 1
    %merge 3 maps into 1 map
	map = label_ch+label_g+label_z;

	%crop the desired animal cluster from the homogenous texture image
    [row,col]=size(label_z);
    zebra=zebra(1:row, 1:col);	%homegenous zebra texture
	zebra(label_z==0)=0;	%mask out non-zebra, so only zebra region of the correct texture left
    cheetah = cheetah(1:row, 1:col);%homegenous cheetah texture
	cheetah(label_ch==0)=0;%mask out non-cheetah, so only cheetah region of the correct texture left
    giraffe = giraffe(1:row, 1:col);%homegenous giraffe texture
	giraffe(label_g==0)=0;%mask out non-giraffe, so only giraffe region of the correct texture left
	final_texture=zebra+cheetah+giraffe;	%sum(union) of the 3 individual clusters together.
%	fig_name=sprintf('right texture:%d',my_count);
%    figure('name', fig_name),imshow(final_texture, 'Border','tight');
	my_count = my_count +1;

	%normalize the range to 0.0-1.0 to show different classes
	label = normalize(map);
end
