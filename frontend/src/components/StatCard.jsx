function StatCard({title,value,subtitle,icon,type="blue",trend}) {
  return (
    <div className="stat-card">
      <div className={`stat-icon ${type}`}>
        {icon}
      </div>

      <div className="stat-content">
        <span className="stat-title">{title}</span>

        <div className="stat-value-row">
          <strong>{value}</strong>

          {trend && (
            <span className={`stat-trend ${trend.type || "positive"}`}>
              {trend.icon} {trend.value}
            </span>
          )}
        </div>

        <span className="stat-subtitle">
          {subtitle}
        </span>
      </div>
    </div>
  );
}

export default StatCard;