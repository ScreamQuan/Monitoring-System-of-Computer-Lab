package com.example.navigationview.ui.sensorview;

import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.TypeReference;
import com.example.navigationview.MyApplication;
import com.example.navigationview.R;
import com.example.navigationview.SwipeRecyclerView;
import org.xutils.common.Callback;
import org.xutils.http.RequestParams;
import org.xutils.x;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class RecyclerViewFragment extends Fragment {

    private RecyclerViewViewModel mViewModel;

    private SwipeRecyclerView recyclerView;
    private MyRecycleViewAdapter adapter;
    //
    private List<HashMap<String, Object>> list = new ArrayList<HashMap<String, Object>>();
    private List<HashMap<String, Object>> mData = new ArrayList<HashMap<String, Object>>();
    private int pageSize = 10;
    String url = new MyApplication().selectbypageurl;

    public static RecyclerViewFragment newInstance() {
        return new RecyclerViewFragment();
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_recyclerview, container, false);

        recyclerView = (SwipeRecyclerView) view.findViewById(R.id.swipeRecyclerView);
        //set color
        recyclerView.getSwipeRefreshLayout()
                .setColorSchemeColors(getResources().getColor(R.color.colorPrimary));

        recyclerView.getRecyclerView().setLayoutManager(new GridLayoutManager(getActivity(), 1));
        //recyclerView.getRecyclerView().setLayoutManager(new StaggeredGridLayoutManager(2, StaggeredGridLayoutManager.VERTICAL));
        adapter=new MyRecycleViewAdapter();
        recyclerView.setAdapter(adapter);
        recyclerView.setOnLoadListener(new SwipeRecyclerView.OnLoadListener() {
            @Override
            public void onRefresh() {

                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        fetchRData();
                    }
                }, 1000);

            }

            @Override
            public void onLoadMore() {
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        fetchMData();
                    }
                }, 1000);
            }
        });

        //???????????????????????????????????????recyclerView.setOnLoadListener()????????????
        //????????????????????????????????????????????????setRefreshing(true),????????????OnLoadListener
        recyclerView.setRefreshing(true);
        return view;

        //??????emptyView
//        TextView textView = new TextView(this);
//        textView.setText("empty view");
//        recyclerView.setEmptyView(textView);
        //??????noMore
        // recyclerView.onNoMore("-- end --");
        //??????????????????
        //recyclerView.onNetChange(true);
        //??????????????????
        //recyclerView.onError("error");


    }
    public void fetchMData() {
        RequestParams params = new RequestParams(url);
        //get
        params.addQueryStringParameter("table", String.valueOf(1));
        params.addQueryStringParameter("offset", String.valueOf(mData.size()));
        params.addQueryStringParameter("pagesize", String.valueOf(pageSize+mData.size()));
        x.http().get(params, new Callback.CommonCallback<String>() {
            @Override
            public void onSuccess(String result) {
                Log.i("RecycleViewFragment", result);
                list = JSON.parseObject(result,
                        new TypeReference<List<HashMap<String, Object>>>() {
                        });
                //mData.clear();
                mData.addAll(list);
                recyclerView.complete();
                adapter.notifyDataSetChanged();
            }

            @Override
            public void onError(Throwable ex, boolean isOnCallback) {

            }

            @Override
            public void onCancelled(CancelledException cex) {

            }

            @Override
            public void onFinished() {

            }
        });

    }

    public void fetchRData() {
        RequestParams params = new RequestParams(url);
        //get
        params.addQueryStringParameter("table", String.valueOf(1));
        params.addQueryStringParameter("offset", String.valueOf(0));
        params.addQueryStringParameter("pagesize", String.valueOf(pageSize));
        x.http().get(params, new Callback.CommonCallback<String>() {
            @Override
            public void onSuccess(String result) {
                Log.i("RecycleViewFragment", result);
                list = JSON.parseObject(result,
                        new TypeReference<List<HashMap<String, Object>>>() {
                        });

                mData.clear();
                mData.addAll(list);
                recyclerView.complete();
                adapter.notifyDataSetChanged();
            }

            @Override
            public void onError(Throwable ex, boolean isOnCallback) {

            }

            @Override
            public void onCancelled(CancelledException cex) {

            }

            @Override
            public void onFinished() {

            }
        });

    }
    class  MyRecycleViewAdapter extends RecyclerView.Adapter<MyRecycleViewAdapter.ViewHolder>
    {
        public  class ViewHolder extends RecyclerView.ViewHolder {
            public ImageView picture;
            public TextView humi;
            public TextView temp;
            public TextView illu;

            public ViewHolder(View convertView) {
                super(convertView);
                picture = (ImageView)convertView.findViewById(R.id.picture);
                humi = (TextView)convertView.findViewById(R.id.valueofhumi);
                temp = (TextView)convertView.findViewById(R.id.valueoftemp);
                illu = (TextView)convertView.findViewById(R.id.valueofillu);
            }
        }
        @NonNull
        @Override
        public MyRecycleViewAdapter.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            View v= LayoutInflater.from(getActivity()).inflate(R.layout.item,parent, false);

            return new ViewHolder(v);
        }

        @Override
        public void onBindViewHolder(@NonNull MyRecycleViewAdapter.ViewHolder holder, final int position) {
              //picture
            //Glide.with(getActivity()).load(new MyApplication().imagebaseurl+mData.get(position).get("picture").toString()).placeholder(R.mipmap.ic_launcher).into(holder.picture);


            //holder.picture.setImageDrawable(getResources().getDrawable(R.mipmap.ic_launcher));
            holder.picture.setImageDrawable(getResources().getDrawable(R.drawable.computer));
            holder.temp.setText((String)mData.get(position).get("temperature"));
            holder.humi.setText((String)mData.get(position).get("humidity"));
            holder.illu.setText((String)mData.get(position).get("illumination"));
        }

        @Override
        public int getItemCount() {
            return mData == null ? 0 : mData.size();
        }
    }
}
