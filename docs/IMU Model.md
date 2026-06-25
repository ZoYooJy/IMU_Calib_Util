
# 误差模型

连续时间：
$$
\begin{align}

\widetilde{\omega}_{ib}^{b}(t) &= [I_{3} + diag(S_g) + M_g] \cdot {\omega}_{ib}^{b}(t) + b_g^b(t) + n_g^b(t) \\

\widetilde{f}_{ib}^{b}(t) &= [I_{3} + diag(S_a) + M_a] \cdot {f}_{ib}^{b}(t) + b_a^b(t) + n_a^b(t) \\

\end{align}
$$


## 误差来源

### 比例因子误差

$$
\begin{align}
s_g &= 
\begin{bmatrix}
s_{g,x} & s_{g,y} & s_{g,z}
\end{bmatrix} \\
s_a &= 
\begin{bmatrix}
s_{a,x} & s_{a,y} & s_{a,z}
\end{bmatrix} \\
\end{align}
$$
- 单位：ppm

### 交叉耦合误差
$$
\begin{align}
M_g &=
\begin{bmatrix}
0 & m_{g,xy} & m_{g,xz} \\
m_{g,yx} & 0 & m_{g,yz} \\
m_{g,zx} & m_{g,zy} & 0 \\
\end{bmatrix} \\

M_a &=
\begin{bmatrix}
0 & m_{a,xy} & m_{a,xz} \\
m_{a,yx} & 0 & m_{a,yz} \\
m_{a,zx} & m_{a,zy} & 0 \\
\end{bmatrix} \\

\end{align}
$$
- 单位：ppm




### 随机噪声

#### 连续时间

连续时间高斯白噪声定义为**零均值高斯随机过程**，自相关函数正比于狄拉克$\delta$函数
$$
\begin{align}

E[n_g(t)] = 0, \quad E[n_g(t) \cdot n_g(\tau)^T] = \Sigma _{gc} \cdot \delta(t - \tau) \\

E[n_a(t)] = 0, \quad E[n_a(t) \cdot n_a(\tau)^T] = \Sigma _{ac} \cdot \delta(t - \tau) \\

\end{align}
$$
- $\Sigma _{gc}$/$\Sigma _{ac}$：$\in R^{3 \times 3}$，双边功率谱密度（PSD）矩阵
	- 各轴==独立且相等==的前提下:
		- $\Sigma _{gc} = diag(\sigma_{g}^2),\quad [(rad/s)^2/Hz]$
		- $\Sigma _{ac} = diag(\sigma_{a}^2),\quad [(m/s^2)^2/Hz]$
- 噪声密度均方根:
	- $\sigma_{g},\quad [rad/s/\sqrt{Hz}]$
	- $\sigma_{a},\quad [m/s^2/\sqrt{Hz}]$

> [!faq]- 狄拉克$\delta$函数
> $\delta$函数：
> - 任意2个不同时刻$t \neq \tau$的噪声值完全不相关
> - 当$t = \tau$时自相关趋于∞，积分面积为有限值（功率谱密度）



#### 离散时间

连续白噪声在任意点的方差理论上无穷大（$\Sigma _{c} \cdot \delta(0) \rightarrow \infty$），因此只能在采样周期内**积分/平均**



|               | 增量模型                                                                                                                                                                        | 平均速率模型                                                                                                                                                             |
| ------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 离散量           | $\Delta \theta_{n,k} \backsim \mathbf{N}(0, \Sigma _{gc} \cdot \Delta t),\quad [rad]$ <br> $\Delta v_{n,k} \backsim \mathbf{N}(0, \Sigma _{ac} \cdot \Delta t),\quad [m/s]$ | $n_{g,k} \backsim \mathbf{N}(0, \dfrac{\Sigma _{gc}}{\Delta t}),\quad [rad/s]$ <br> $n_{a,k} \backsim \mathbf{N}(0, \dfrac{\Sigma _{ac}}{\Delta t}),\quad [m/s^2]$ |
| 协方差           | $\Sigma _{gc} \cdot \Delta t$ <br> $\Sigma _{ac} \cdot \Delta t$                                                                                                            | $\dfrac{\Sigma _{gc}}{\Delta t}$ <br> $\dfrac{\Sigma _{ac}}{\Delta t}$                                                                                             |
| 与$\Delta t$关系 | 协方差 $\propto \Delta t$（采样越慢，积分噪声越大）                                                                                                                                         | 协方差 $\propto \dfrac{1}{\Delta t}$（采样越慢，平均后噪声越小）                                                                                                                    |
| 物理含义          | 白噪声在采样周期内的==积分增量==（面积量）                                                                                                                                                     | 白噪声在采样周期内的==时间平均==（速率量）                                                                                                                                            |
| 作用环节          | 进入运动学状态传播（预积分），作为==过程噪声==加入 $Q$ 矩阵                                                                                                                                          | 进入离散测量方程，作为==测量噪声==加入 $R$ 矩阵                                                                                                                                       |


##### 增量模型

白噪声在$\Delta t = t_{k+1} - t_k$内的==积分增量==当作随机变量，直接加入运动学状态传播（预积分）：
$$
\begin{align}
\Delta \theta_{n, k} &= \int _{t_k}^{t_{k+1}} n_{g}(t) dt,\quad [rad] \\

\Delta v_{n,k} &= \int _{t_k}^{t_{k+1}} n_{a}(t) dt,\quad [m/s] \\

\end{align}
$$

增量对应的协方差：
$$
\begin{align}

\mathbf{Cov}(\Delta \theta_{n,k}) &= \mathbf{E}[(\int _{t_k}^{t_{k+1}} n_{g}(t) dt) \cdot (\int _{t_k}^{t_{k+1}} n_{g}(\tau) d\tau)^T] \\
&= \int _{t_k}^{t_{k+1}} \int _{t_k}^{t_{k+1}} \mathbf{E}[(n_{g}(t)) \cdot (n_{g}(\tau))^T] dt d\tau \\
&= \int _{t_k}^{t_{k+1}} \int _{t_k}^{t_{k+1}} \Sigma _{gc} \cdot \delta(t - \tau) dt d\tau \\
&= \Sigma _{gc} \cdot \int _{t_k}^{t_{k+1}} 1 d\tau \\
&= \Sigma _{gc} \cdot \tau \vert_{t_k}^{t_{k+1}} \\
&= \Sigma _{gc} \cdot (t_{k+1} - t_{k}) \\
&= \Sigma _{gc} \cdot \Delta t,\quad (rad)^2 = [(rad/s)^2/Hz] \times [s] \\
\\
\mathbf{Cov}(\Delta v_{n,k}) &= \mathbf{E}[(\int _{t_k}^{t_{k+1}} n_{a}(t) dt) \cdot (\int _{t_k}^{t_{k+1}} n_{a}(\tau) d\tau)^T] \\
&= \int _{t_k}^{t_{k+1}} \int _{t_k}^{t_{k+1}} \mathbf{E}[(n_{a}(t)) \cdot (n_{a}(\tau))^T] dt d\tau \\
&= \int _{t_k}^{t_{k+1}} \int _{t_k}^{t_{k+1}} \Sigma _{ac} \cdot \delta(t - \tau) dt d\tau \\
&= \Sigma _{ac} \cdot \int _{t_k}^{t_{k+1}} 1 d\tau \\
&= \Sigma _{ac} \cdot \tau \vert_{t_k}^{t_{k+1}} \\
&= \Sigma _{ac} \cdot (t_{k+1} - t_{k}) \\
&= \Sigma _{ac} \cdot \Delta t,\quad (m/s)^2 = [(m/s^2)^2/Hz] \times [s] \\
\end{align}
$$
即：
$$
\begin{align}

\Delta \theta_{n,k} & \backsim \mathbf{N}(0,\quad \Sigma _{gc} \cdot \Delta t) \\

\Delta v_{n,k} & \backsim \mathbf{N}(0,\quad \Sigma _{ac} \cdot \Delta t) \\

\end{align}
$$


##### 平均速率模型

白噪声的==时间平均==作为离散时刻的测量噪声，直接加入离散测量方程（R矩阵）：
$$
\begin{align}
n_{g,k} &= \frac{1}{\Delta t} \int _{t_k}^{t_{k+1}} n_{g}(t) dt = \frac{\Delta \theta_{n,k}}{\Delta t},\quad [rad/s] \\

n_{a,k} &= \frac{1}{\Delta t} \int _{t_k}^{t_{k+1}} n_{a}(t) dt = \frac{\Delta v_{n,k}}{\Delta t},\quad [m/s^2] \\

\end{align}
$$
根据随机变量线性变换的协方差：
$$
\begin{align}

\mathbf{Cov}(n_{g,k}) &= \frac{1}{\Delta t^2} \cdot \mathbf{Cov}(\Delta \theta_{n,k}) \\
&= \frac{1}{\Delta t^2} \cdot (\Sigma _{gc} \cdot \Delta t) \\
&= \frac{\Sigma _{gc}}{\Delta t},\quad (rad/s)^2 = [(rad/s)^2/Hz] \times [s^{-1}] \\
\\
\mathbf{Cov}(n_{a,k}) &= \frac{1}{\Delta t^2} \cdot \mathbf{Cov}(\Delta v_{n,k}) \\
&= \frac{1}{\Delta t^2} \cdot (\Sigma _{ac} \cdot \Delta t) \\
&= \frac{\Sigma _{ac}}{\Delta t},\quad (m/s^2)^2 = [(m/s^2)^2/Hz] \times [s^{-1}] \\

\end{align}
$$
即：
$$
\begin{align}

n_{g,k} \backsim \mathbf{N}(0,\quad \frac{\Sigma _{gc}}{\Delta t}) \\

n_{a,k} \backsim \mathbf{N}(0,\quad \frac{\Sigma _{ac}}{\Delta t}) \\

\end{align}
$$




### 零偏

- 传感器在无输入时的非零输出，随时间缓慢漂移，最重要的低频误差源。



#### 随机游走(维纳过程的导数)

- 漂移无界，方差随时间线性增长

##### 连续时间

连续时间微分方程：
$$
\begin{align}

\dot{b}_g(t) &= n_{b_g}(t) \\

\dot{b}_a(t) &= n_{b_a}(t) \\

\end{align}
$$
- $\dot{b}_g(t)$: 角速度零偏变化率，$rad/s^2$
- $\dot{b}_a(t)$: 加速度零偏变化率，$m/s^3$

驱动白噪声$n_{b_g}(t),n_{b_a}(t)$均为零均值高斯白噪声过程，自相关为：
$$
\begin{align}
\mathbf{E}(n_{b_g}(t)) &= 0,\quad \mathbf{E}((n_{b_g}(t)) \cdot (n_{b_g}(\tau))^T) = \Sigma _{b_g,c} \cdot \delta (t-\tau) \\

\mathbf{E}(n_{b_a}(t)) &= 0,\quad \mathbf{E}((n_{b_a}(t)) \cdot (n_{b_a}(\tau))^T) = \Sigma _{b_a,c} \cdot \delta (t-\tau) \\
\end{align}
$$
- $\Sigma _{b_g,c}$/$\Sigma _{b_a,c}$：$\in R^{3 \times 3}$，双边功率谱密度（PSD）矩阵
	- 各轴==独立且相等==的前提下:
		- $\Sigma _{b_g,c} = diag({\sigma_{b_g}^{rw}}^2),\quad [(rad/s^2)^2/Hz]$
		- $\Sigma _{b_a,c} = diag({\sigma_{b_a}^{rw}}^2),\quad [(m/s^3)^2/Hz]$
- 噪声密度均方根:
	- $\sigma_{b_g}^{rw},\quad [rad/s^2/\sqrt{Hz}]$
	- $\sigma_{b_a}^{rw},\quad [m/s^3/\sqrt{Hz}]$


- 零偏的增量与白噪声在时间上的积分成正比，即$b_g/b_a$本身是随机游走（维纳过程）
- 由于缺乏回归项，零偏可以从任意值开始无界漂移

##### 离散时间

对连续微分方程进行积分：
$$
\begin{align}
b_{g,k+1} &= b_{g,k} + \int _{t_{k}}^{t_{k+1}} n_{b_g}(t) dt,\quad rad/s \\

b_{a,k+1} &= b_{a,k} + \int _{t_{k}}^{t_{k+1}} n_{b_a}(t) dt,\quad m/s^2 \\

\end{align}
$$
驱动噪声增量（只能为增量模型，是积分的结果，保证状态量和噪声增量的量纲一致）:
$$
\begin{align}
\omega _{g,k} &= \int _{t_{k}}^{t_{k+1}} n_{b_g}(t) dt,\quad rad/s \\
\omega _{a,k} &= \int _{t_{k}}^{t_{k+1}} n_{b_a}(t) dt,\quad m/s^2 \\
\end{align}
$$

对应的协方差为：
$$
\begin{align}

\mathbf{Cov}(\omega _{g,k}) &= \mathbf{E}[(\int _{t_k}^{t_{k+1}} n_{b_g}(t) dt) \cdot (\int _{t_k}^{t_{k+1}} n_{b_g}(\tau) d\tau)^T] \\
&= \int _{t_k}^{t_{k+1}} \int _{t_k}^{t_{k+1}} \mathbf{E}[(n_{b_g}(t)) \cdot (n_{b_g}(\tau))^T] dt d\tau \\
&= \int _{t_k}^{t_{k+1}} \int _{t_k}^{t_{k+1}} \Sigma _{b_g,c} \cdot \delta(t - \tau) dt d\tau \\
&= \Sigma _{b_g,c} \cdot \int _{t_k}^{t_{k+1}} 1 d\tau \\
&= \Sigma _{b_g,c} \cdot \tau \vert_{t_k}^{t_{k+1}} \\
&= \Sigma _{b_g,c} \cdot (t_{k+1} - t_{k}) \\
&= \Sigma _{b_g,c} \cdot \Delta t,\quad (rad/s)^2 = [(rad/s^2)^2/Hz] \times [s] \\
\\
\mathbf{Cov}(\omega _{a,k}) &= \mathbf{E}[(\int _{t_k}^{t_{k+1}} n_{b_a}(t) dt) \cdot (\int _{t_k}^{t_{k+1}} n_{b_a}(\tau) d\tau)^T] \\
&= \int _{t_k}^{t_{k+1}} \int _{t_k}^{t_{k+1}} \mathbf{E}[(n_{b_a}(t)) \cdot (n_{b_a}(\tau))^T] dt d\tau \\
&= \int _{t_k}^{t_{k+1}} \int _{t_k}^{t_{k+1}} \Sigma _{b_a,c} \cdot \delta(t - \tau) dt d\tau \\
&= \Sigma _{b_a,c} \cdot \int _{t_k}^{t_{k+1}} 1 d\tau \\
&= \Sigma _{b_a,c} \cdot \tau \vert_{t_k}^{t_{k+1}} \\
&= \Sigma _{b_a,c} \cdot (t_{k+1} - t_{k}) \\
&= \Sigma _{b_a,c} \cdot \Delta t,\quad (m/s^2)^2 = [(m/s^3)^2/Hz] \times [s] \\

\end{align}
$$

因此，离散随机游走模型为：
$$
\begin{align}
b_{g,k+1} &= b_{g,k} + \omega_{g,k},\quad  \omega_{g,k} \thicksim \mathbf{N}(0, \Sigma _{b_g,c} \cdot \Delta t) \\

b_{a,k+1} &= b_{a,k} + \omega_{a,k},\quad \omega_{a,k} \thicksim \mathbf{N}(0, \Sigma _{b_a,c} \cdot \Delta t) \\
\end{align}
$$

#### FOGM

- 有限稳态方差和指数衰减的自相关

##### 连续时间

连续时间微分方程：
$$
\begin{align}
\dot{b}_g(t) &= - \frac{1}{\tau _c} \cdot b_{g}(t) + n_{b_g}(t) \\

\dot{b}_a(t) &= - \frac{1}{\tau _c} \cdot b_{a}(t) + n_{b_a}(t) \\
\end{align}
$$
驱动白噪声$n_{b_g}(t),n_{b_a}(t)$均为零均值高斯白噪声过程，自相关为：
$$
\begin{align}
\mathbf{E}(n_{b_g}(t)) &= 0,\quad \mathbf{E}((n_{b_g}(t)) \cdot (n_{b_g}(\tau))^T) = Q_{b_g,c} \cdot \delta (t-\tau) \\

\mathbf{E}(n_{b_a}(t)) &= 0,\quad \mathbf{E}((n_{b_a}(t)) \cdot (n_{b_a}(\tau))^T) = Q_{b_a,c} \cdot \delta (t-\tau) \\
\end{align}
$$

##### 离散时间

对于一阶线性随机方程，具有解析解，对于区间$[t_k, t_{k+1}]$，具体步骤如下：

1. 移项：
$$
\begin{align}
\dot{b}_g(t) &+ \frac{1}{\tau _c} \cdot b_{g}(t) = n_{b_g}(t) \\
\dot{b}_a(t) &+ \frac{1}{\tau _c} \cdot b_{a}(t) = n_{b_a}(t) \\
\end{align}
$$

2. 构造积分因子，基于**乘积求导法则**，凑出乘积求导法则的逆运算，将$b(t)/\dot{b}(t)$构造为一个整体
	- 令$u(t) = e^{\frac{t}{\tau_{c}}}$

> [!faq]- 如何构造积分因子
> 乘积求导法则：$\frac{d[A(t) \cdot B(t)]}{dt} = \dot{A}(t) \cdot B(t) + A(t) \cdot \dot{B}(t)$
> 对于原公式，两边分别乘以一个未知函数：$u(t) \cdot \dot{b}(t) + u(t) \cdot \frac{1}{\tau_c} \cdot b(t) = u(t) \cdot \omega(t)$
> 基于 乘积求导法则，有：$\frac{d[u(t) \cdot b(t)]}{dt} = \dot{u}(t) \cdot b(t) + u(t) \cdot \dot{b}(t)$
> 若要2个公式等价，则有：$\dot{u}(t) \cdot b(t) = u(t) \cdot \frac{1}{\tau_c} \cdot b(t)$
> 当$b(t) != 0$时，得到关于$u(t)$的微分方程：$\dot{u}(t) = \frac{1}{\tau_c} \cdot u(t)$
> 解得：$u(t) = e^{\frac{t}{\tau_c}}$

3. 微分方程同时乘以积分因子：
$$
\begin{align}

e^{\frac{t}{\tau_{c}}} \cdot \dot{b_g}(t) + e^{\frac{t}{\tau_{c}}} \cdot \frac{1}{\tau_{c}} \cdot b_g(t) &= e^{\frac{t}{\tau_{c}}} \cdot n_{b_g}(t) \\

\frac{d[e^{\frac{t}{\tau_{c}}} \cdot b_g(t)]}{dt} &= e^{\frac{t}{\tau_{c}}} \cdot n_{b_g}(t) \\
------------ \\

e^{\frac{t}{\tau_{c}}} \cdot \dot{b_a}(t) + e^{\frac{t}{\tau_{c}}} \cdot \frac{1}{\tau_{c}} \cdot b_a(t) &= e^{\frac{t}{\tau_{c}}} \cdot n_{b_a}(t) \\

\frac{d[e^{\frac{t}{\tau_{c}}} \cdot b_a(t)]}{dt} &= e^{\frac{t}{\tau_{c}}} \cdot n_{b_a}(t) \\

\end{align}
$$
4. 在时间区间$[t_k,t_{k+1}]$进行定积分：
$$
\begin{align}

\int_{t_k}^{t_{k+1}} \frac{d[e^{\frac{\tau}{\tau_{c}}} \cdot b_g(\tau)]}{dt} d\tau &= \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_g}(\tau) d\tau \\

e^{\frac{t_{k+1}}{\tau_{c}}} \cdot b_{g}(t_{k+1}) - e^{\frac{t_{k}}{\tau_{c}}} \cdot b_{g}(t_{k}) &= \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_g}(\tau) d\tau \\

------------ \\

\int_{t_k}^{t_{k+1}} \frac{d[e^{\frac{\tau}{\tau_{c}}} \cdot b_a(\tau)]}{dt} d\tau &= \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_a}(\tau) d\tau \\

e^{\frac{t_{k+1}}{\tau_{c}}} \cdot b_{a}(t_{k+1}) - e^{\frac{t_{k}}{\tau_{c}}} \cdot b_{a}(t_{k}) &= \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_a}(\tau) d\tau \\

\end{align}
$$
	- 移项并同时乘以$e^{-\frac{t_{k+1}}{\tau_{c}}}$：
$$
\begin{align}

b_g(t_{k+1}) &= e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot e^{\frac{t_{k}}{\tau_{c}}} \cdot b_g(t_{k}) + e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_g}(\tau) d\tau \\

b_g(t_{k+1}) &= e^{-\frac{t_{k+1} - t_k}{\tau_{c}}} \cdot b_g(t_{k}) + e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_g}(\tau) d\tau \\
----\\

b_a(t_{k+1}) &= e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot e^{\frac{t_{k}}{\tau_{c}}} \cdot b_a(t_{k}) + e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_a}(\tau) d\tau \\

b_a(t_{k+1}) &= e^{-\frac{t_{k+1} - t_k}{\tau_{c}}} \cdot b_a(t_{k}) + e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_a}(\tau) d\tau \\

\end{align}
$$
5. 离散化表示
	- 采样间隔$\Delta t = t_{k+1} - t_k$
	- 离散状态量$b_{k+1} = b(t_{k+1}), b_{k} = b(t_{k})$
$$
\begin{align}

b_{g,k+1} &= e^{-\frac{\Delta t}{\tau_c}} \cdot b_{g,k} + e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_g}(\tau) d\tau \\

b_{a,k+1} &= e^{-\frac{\Delta t}{\tau_c}} \cdot b_{a,k} + e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_a}(\tau) d\tau \\

\end{align}
$$
	- ==状态转移矩阵==$\Phi_{b} = e^{-\frac{\Delta t}{\tau_c}}$
	- 离散过程噪声$\zeta _{k}$：
$$
\begin{align}
\zeta _{g,k} &= e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_g}(\tau) d\tau \\

\zeta _{a,k} &= e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{\tau}{\tau_{c}}} \cdot n_{b_a}(\tau) d\tau \\

\end{align}
$$


6. $\zeta _{k}$的协方差：
	- $n_{b_g}/n_{b_a}$为零均值高斯白噪声且初始状态独立 => $\zeta _{g,k} / \zeta _{a,k}$为零均值高斯随机向量
$$
\begin{align}

\mathbf{Cov}(\zeta _{g,k}) &= \mathbf{E}((e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{\mu}{\tau_{c}}} \cdot n_{b_g}(\mu) d\mu) \cdot (e^{-\frac{t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{\nu}{\tau_{c}}} \cdot n_{b_g}(\nu) d\nu)^T) \\

&= e^{-\frac{2 \cdot t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} \int_{t_k}^{t_{k+1}} e^{\frac{\mu + \nu}{\tau_{c}}} \cdot \mathbf{E}((n_{b_g}(\mu)) \cdot (n_{b_g}(\nu))^T) d\mu d\nu \\

&= e^{-\frac{2 \cdot t_{k+1}}{\tau_{c}}} \cdot \int_{t_k}^{t_{k+1}} \int_{t_k}^{t_{k+1}} e^{\frac{\mu + \nu}{\tau_{c}}} \cdot Q_{b_g,c} \cdot \delta(\mu - \nu) d\mu d\nu \\

&= e^{-\frac{2 \cdot t_{k+1}}{\tau_{c}}} \cdot Q_{b_g,c} \cdot \int_{t_k}^{t_{k+1}} \int_{t_k}^{t_{k+1}} e^{\frac{\mu + \nu}{\tau_{c}}} \cdot \delta(\mu - \nu) d\mu d\nu \\

&= e^{-\frac{2 \cdot t_{k+1}}{\tau_{c}}} \cdot Q_{b_g,c} \cdot \int_{t_k}^{t_{k+1}} e^{\frac{2 \cdot \nu}{\tau_{c}}} d\nu \\

&= e^{-\frac{2 \cdot t_{k+1}}{\tau_{c}}} \cdot Q_{b_g,c} \cdot [\frac{\tau_c}{2} \cdot e^{\frac{2 \cdot \nu}{\tau_{c}}}] |_{t_k}^{t_{k+1}} \\

&= \frac{\tau_c}{2} \cdot Q_{b_g,c} \cdot (1 - e^{\frac{-2 \cdot \Delta t}{\tau_c}}) \\ 
---- \\

\mathbf{Cov}(\zeta _{a,k}) &= \frac{\tau_c}{2} \cdot Q_{b_a,c} \cdot (1 - e^{\frac{-2 \cdot \Delta t}{\tau_c}}) \\ 

\end{align}
$$

因此，离散一阶高斯-马尔可夫模型为：
$$
\begin{align}

b_{g,k+1} &= e^{- \frac{\Delta t}{\tau_c}} \cdot b_{g,k} + \zeta _{g,k},\quad \zeta _{g,k} \thicksim\mathbf{N}(0, \frac{\tau_c}{2} \cdot Q_{b_g,c} \cdot (1 - e^{\frac{-2 \cdot \Delta t}{\tau_c}})) \\

b_{a,k+1} &= e^{- \frac{\Delta t}{\tau_c}} \cdot b_{a,k} + \zeta _{a,k},\quad \zeta _{a,k} \thicksim\mathbf{N}(0, \frac{\tau_c}{2} \cdot Q_{b_a,c} \cdot (1 - e^{\frac{-2 \cdot \Delta t}{\tau_c}})) \\

\end{align}
$$

则对应的离散过程噪声方差为：
$$
\begin{align}
Q_{b_g,d} &= \frac{\tau_c}{2} \cdot Q_{b_g,c} \cdot (1 - e^{\frac{-2 \cdot \Delta t}{\tau_c}}) \\

Q_{b_a,d} &= \frac{\tau_c}{2} \cdot Q_{b_a,c} \cdot (1 - e^{\frac{-2 \cdot \Delta t}{\tau_c}}) \\

\end{align}
$$

对于FOGM模型，稳态方差${\sigma_{b}^{ss}}^{2}$与连续驱动噪声谱密度$Q_c$满足李亚普诺夫方程稳态条件：
$$
\begin{align}
0 &= - \frac{2}{\tau_c} \cdot {\sigma_{b_g}^{ss}}^{2} + Q_{b_g,c} \\
0 &= - \frac{2}{\tau_c} \cdot {\sigma_{b_a}^{ss}}^{2} + Q_{b_a,c} \\
&=> \\
Q_{b_g,c} &= \frac{2 \cdot {\sigma_{b_g}^{ss}}^{2}}{\tau_c} \\
Q_{b_a,c} &= \frac{2 \cdot {\sigma_{b_a}^{ss}}^{2}}{\tau_c} \\
\end{align}
$$
则离散过程噪声方差简化为：
$$
\begin{align}
Q_{b_g,d} &= {\sigma_{b_g}^{ss}}^{2} \cdot (1 - e^{\frac{-2 \cdot \Delta t}{\tau_c}}) \\
Q_{b_a,d} &= {\sigma_{b_a}^{ss}}^{2} \cdot (1 - e^{\frac{-2 \cdot \Delta t}{\tau_c}}) \\
\end{align}
$$

当$\Delta t << \tau_c$时，有$e^{\frac{-2 \cdot \Delta t}{\tau_c}} \approx 1 - \frac{2 \cdot \Delta t}{\tau_c}$
离散过程噪声方差进一步简化为：
$$
\begin{align}
Q_{b_g,d} &= {\sigma_{b_g}^{ss}}^{2} \cdot (1 - e^{\frac{-2 \cdot \Delta t}{\tau_c}}) \\
&= {\sigma_{b_g}^{ss}}^{2} \cdot [1 - (1 - \frac{2 \cdot \Delta t}{\tau_c})] \\
&= Q_{b_g,c} \cdot \Delta t \\
\\
Q_{b_a,d} &= {\sigma_{b_a}^{ss}}^{2} \cdot (1 - e^{\frac{-2 \cdot \Delta t}{\tau_c}}) \\
&= {\sigma_{b_a}^{ss}}^{2} \cdot [1 - (1 - \frac{2 \cdot \Delta t}{\tau_c})] \\
&= Q_{b_a,c} \cdot \Delta t \\

\end{align}
$$
