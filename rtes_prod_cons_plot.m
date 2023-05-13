% Create a table to save all the mean values
mean_values = zeros(10);
% Create a table with elements the queue size
queue_size = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100];

% Load all the data from the txt files
data_10 = readtable('time_measurement_q10_con10.txt');
data_1 = data_10{:,1}; 
data_20 = readtable('time_measurement_q20_con10.txt');
data_2 = data_20{:,1};
data_30 = readtable('time_measurement_q30_con10.txt');
data_3 = data_30{:,1};
data_40 = readtable('time_measurement_q40_con10.txt');
data_4 = data_40{:,1};
data_50 = readtable('time_measurement_q50_con10.txt');
data_5 = data_50{:,1};
data_60 = readtable('time_measurement_q60_con10.txt');
data_6 = data_60{:,1};
data_70 = readtable('time_measurement_q70_con10.txt');
data_7 = data_70{:,1};
data_80 = readtable('time_measurement_q80_con10.txt');
data_8 = data_80{:,1};
data_90 = readtable('time_measurement_q90_con10.txt');
data_9 = data_90{:,1};
data_100 = readtable('time_measurement_q100_con10.txt');
data_10 = data_100{:,1};

% Calculate the mean value of each file
mean_values(1) = mean(data_1);
mean_values(2) = mean(data_2);
mean_values(3) = mean(data_3);
mean_values(4) = mean(data_4);
mean_values(5) = mean(data_5);
mean_values(6) = mean(data_6);
mean_values(7) = mean(data_7);
mean_values(8) = mean(data_8);
mean_values(9) = mean(data_9);
mean_values(10) = mean(data_10);


%Plot the mean value of each number of queue
figure(1);
plot(queue_size,mean_values);
title('Producer-Consumer element waiting time');
xlabel('Queue Size') 
ylabel('Waiting Time(us)') 
