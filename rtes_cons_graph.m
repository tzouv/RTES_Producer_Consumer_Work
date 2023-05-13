% Create a table to save all the mean values
mean_values = zeros(6);
% Create a table with elements the queue size
con_size = [4, 8, 10, 12, 16, 20];

% Load all the data from the txt files
con_4 = readtable('time_measurement_q20_con4.txt');
data_1 = con_4{:,1}; 
con_8 = readtable('time_measurement_q20_con8.txt');
data_2 = con_8{:,1}; 
con_10 = readtable('time_measurement_q20_con10.txt');
data_3 = con_10{:,1}; 
con_12 = readtable('time_measurement_q20_con12.txt');
data_4 = con_12{:,1}; 
con_16 = readtable('time_measurement_q20_con16.txt');
data_5 = con_16{:,1}; 
con_20 = readtable('time_measurement_q20_con20.txt');
data_6 = con_20{:,1}; 

% Calculate the mean value of each file
mean_values(1) = mean(data_1);
mean_values(2) = mean(data_2);
mean_values(3) = mean(data_3);
mean_values(4) = mean(data_4);
mean_values(5) = mean(data_5);
mean_values(6) = mean(data_6);

%Plot the mean value of each number of queue
figure(1);
plot(con_size,mean_values);
title('Producer-Consumer element waiting time');
xlabel('Con Size'); 
ylabel('Waiting Time(us)');