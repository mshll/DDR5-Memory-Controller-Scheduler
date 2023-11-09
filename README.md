# ECE485/585 Final Project

Memory Controller Simulator

## Topological Address Mapping
<div class="tg-wrap"><table class="tg">
<tbody>
  <tr>
    <td>34</td>
    <td>33</td>
    <td>32</td>
    <td>31</td>
    <td>30</td>
    <td>29</td>
    <td>28</td>
    <td>27</td>
    <td>26</td>
    <td>25</td>
    <td>24</td>
    <td>23</td>
    <td>22</td>
    <td>21</td>
    <td>20</td>
    <td>19</td>
    <td>18</td>
    <td>17</td>
    <td>16</td>
    <td>15</td>
    <td>14</td>
    <td>13</td>
    <td>12</td>
    <td>11</td>
    <td>10</td>
    <td>9</td>
    <td>8</td>
    <td>7</td>
    <td>6</td>
    <td>5</td>
    <td>4</td>
    <td>3</td>
    <td>2</td>
    <td>1</td>
    <td>0</td>
  </tr>
  <tr>
    <td colspan="16" rowspan="2">Row</td>
    <td colspan="7" rowspan="2">Column [10:4]</td>
    <td colspan="3" rowspan="2">Bank Group</td>
    <td colspan="2" rowspan="2">Bank</td>
    <td rowspan="2">Channel</td>
    <td colspan="4" rowspan="2">Column [3:0]</td>
    <td colspan="2" rowspan="2">Byte Select</td>
  </tr>
  <tr>
  </tr>
</tbody>
</table></div>

## Coding Conventions

variables_use_snake_case
```
int array_index = 0;
```

functions_use_snake_case
```
void foo_bar();
```

StructUsePascalCase
```
typedef struct PersonInfo {
    char *name;

} PersonInfo_t;
```

EnumsUsePascalCase
```
enum PersonInfo {
    NAME = 1,
    AGE = 2
};
```

**don't**:
```
foo_bar(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
```

**do**:
```
foo_bar(
    param1,
    param2,
    param3,
    param4,
    param5,
    param6,
    param7,
    param8,
    param9,
    param10);
```